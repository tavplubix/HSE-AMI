# ZooKeeper Codewalk

## Реплицируемый Автомат



### Найдите код автомата, который реплицирует ZooKeeper
Класс [DataTree](https://github.com/apache/zookeeper/blob/794adf11220151496a1ff877df3d2627719c9505/zookeeper-server/src/main/java/org/apache/zookeeper/server/DataTree.java#L93) представляет дерево, которое реплицируется зукипером.
Ещё есть класс [ZKDatabase](https://github.com/apache/zookeeper/blob/553639378d5cf86c2488afff4586e5e4cce38061/zookeeper-server/src/main/java/org/apache/zookeeper/server/ZKDatabase.java#L68), который отвечает за хранение состояния сервера (в том числе лога репликации со снепшотами [FileTxnSnapLog](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/persistence/FileTxnSnapLog.java#L51), информации о сессиях и самого дерева). Ну т.е. можно считать, что зукипер реплициует лог, а можно считать, что реплицирует дерево с помощью лога операций над деревом.

### Какие атрибуты есть у узлов дерева? Как они используются?
Узлы дерева - [DataNode](https://github.com/apache/zookeeper/blob/fe940cdd8fb23ba09684cefb73233d570f4a20fa/zookeeper-server/src/main/java/org/apache/zookeeper/server/DataNode.java#L40). Атрибуты:

- `data` - данные, записанные в узел
- `acl` - Access Control List
- `stat` - [всякие метаданные](https://github.com/apache/zookeeper/blob/553639378d5cf86c2488afff4586e5e4cce38061/zookeeper-jute/src/main/resources/zookeeper.jute#L43) вроде времени создания и изменения
- `children` - множество имён дочерних узлов
- (ещё есть `digest` и `digestCached` - `CRC32`-хэш от данных, метаданных и пути; и флаг, нужно ли его пересчитывать)

### Есть ли в дереве служебные узлы?
Есть `/zookeeper`, и [например](https://github.com/apache/zookeeper/blob/794adf11220151496a1ff877df3d2627719c9505/zookeeper-server/src/main/java/org/apache/zookeeper/server/DataTree.java#L125) `/zookeeper/config` и `/zookeeper/quota` внутри него.


## Выборы лидера

### Найдите интерфейс компонента, который отвечает за выбор лидера

[Election](https://github.com/apache/zookeeper/blob/fe940cdd8fb23ba09684cefb73233d570f4a20fa/zookeeper-server/src/main/java/org/apache/zookeeper/server/quorum/Election.java#L21). От него наследуются две реализации [FastLeaderElection](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/quorum/FastLeaderElection.java#L50) (основная) и [AuthFastLeaderElection](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/quorum/AuthFastLeaderElection.java#L51) (deprecated)

### Найдите реализацию алгоритма выбора лидера. Как устроен этот алгоритм?
[FastLeaderElection](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/quorum/FastLeaderElection.java#L50)

Вообще описание алгоритма есть в книге на странице 157, но оно неправильное. В частности, в коде есть некоторый `electionEpoch`, которого нет в книге. А ещё, если я правильно понял, что написано в книге, то для выбора нового голоса используется только "current vote of the receiver" и "the values of the receiver itself", т.е. полученный от другого хоста голос как бы не используется, что странно и не может рабоать. А ещё в истории нашлись прекрасные коммиты вроде [ZOOKEEPER-1081. modify leader/follower code to correctly deal with new leader](https://github.com/apache/zookeeper/commit/776db7808e27624aa125f1314b7fbb2196837507), который как раз добавляет эпоху, что несколько пошатывет веру в ZooKeeper (впрочем, это было давно, и, наверно, все такие вещи уже пофиксили и теперь всё точно работает).

Судя по коду, работает это как-то так:
- Голос - это кортеж `(electionEpoch, zxid, hostid)`, `electionEpoch` - номер эпохи (увеличивается при каждых выборах, пишется в лог), `zxid` - номер последней транзакции в логе реплики (чтобы побеждала реплика, у которой больше записей), hostid - tie-breaker
- Сначала каждая реплика голосует за себя.
- Затем реплики получают голоса от других, если они увидели голос больше своего, то меняют совй голос на больший.
- Реплика становится лидером, когда понимает, что за неё проголосовал кворум.


### Какие роли / состояния есть в алгоритме, помимо лидера?
[Состояния](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/quorum/QuorumPeer.java#L416):
- LOOKING - идут выборы лидера
- LEADING - эта реплика лидер, она обслуживает запросы
- FOLLOWING - эта реплика не лидер, форвардит запросы на лидера, подтверждает лидеру, что сохранила себе записи
- OBSERVING - похоже на FOLLOWING но не входит в кворум, т.е. реплика в таком состоянии не участвует в выборах (не голосует и не может стать лидером); лидеру не требуется подтверждение записи от этой реплики (это состояние конфигурируется)

## Конвейер обработки запроса


### Из каких RequestProcessor-ов состоит конвейер обработки запросов на реплике-лидере? За что отвечает каждый компонент в этом конвейере?
[Вот](https://github.com/apache/zookeeper/blob/fe940cdd8fb23ba09684cefb73233d570f4a20fa/zookeeper-server/src/main/java/org/apache/zookeeper/server/quorum/LeaderZooKeeperServer.java#L65). Процессоры в порядке вложенности:
- `LeaderRequestProcessor` - responsible for performing local session upgrade
- `PrepRequestProcessor` - sets up any transactions associated with requests that change the state of the system
- `ProposalRequestProcessor` - simply forwards requests to an AckRequestProcessor and SyncRequestProcessor
- `CommitProcessor` -  matches the incoming committed requests with the locally submitted requests
- `ToBeAppliedRequestProcessor` - simply maintains the toBeApplied list
- `FinalRequestProcessor` - actually applies any transaction associated with a request and services any queries

Ещё два процессора, которые выполняются в `ProposalRequestProcessor` после `CommitProcessor` (для лидера):
- `SyncRequestProcessor` - sync request to disk and forward it to `AckRequestProcessor` which send ack back to itself
- `AckRequestProcessor` - simply forwards a request from a previous stage to the leader as an ACK

### Найдите код, который диспетчеризует обработку пользовательских операций
Вот [тут](https://github.com/apache/zookeeper/blob/553639378d5cf86c2488afff4586e5e4cce38061/zookeeper-server/src/main/java/org/apache/zookeeper/server/PrepRequestProcessor.java#L302) и  [тут](https://github.com/apache/zookeeper/blob/ae68c7d50e4057ef3f9b99ac0cd0200a8bd5f235/zookeeper-server/src/main/java/org/apache/zookeeper/server/FinalRequestProcessor.java#L199) огромные свитчкейсы на разные типы пользовательских запросов (один подготовливает транзакцию, другой выполняет).

### Найдите код, который строит имя sequential-узла при его создании
[Вот](https://github.com/apache/zookeeper/blob/553639378d5cf86c2488afff4586e5e4cce38061/zookeeper-server/src/main/java/org/apache/zookeeper/server/PrepRequestProcessor.java#L609)

### Найдите код, который строит транзакцию для операции setData
[Вот](https://github.com/apache/zookeeper/blob/553639378d5cf86c2488afff4586e5e4cce38061/zookeeper-server/src/main/java/org/apache/zookeeper/server/PrepRequestProcessor.java#L349)

### Найдите код, который применяет построенную транзакцию к дереву
[Вот](https://github.com/apache/zookeeper/blob/794adf11220151496a1ff877df3d2627719c9505/zookeeper-server/src/main/java/org/apache/zookeeper/server/DataTree.java#L874)

### Как обслуживаются read-only запросы? Найдите код, который формирует ответ на запрос getData
[Вот](https://github.com/apache/zookeeper/blob/ae68c7d50e4057ef3f9b99ac0cd0200a8bd5f235/zookeeper-server/src/main/java/org/apache/zookeeper/server/FinalRequestProcessor.java#L636). Транзакций не создаётся.

### Найдите код, который отправляет ответ клиенту
[Вот](https://github.com/apache/zookeeper/blob/ae68c7d50e4057ef3f9b99ac0cd0200a8bd5f235/zookeeper-server/src/main/java/org/apache/zookeeper/server/FinalRequestProcessor.java#L588)

### Что делает команда sync? Найди код обработки команды sync.
В `SyncRequestProcessor`. Эта команда форсит загрузку лога на фолловера (или обсёрвера) с лидера, чтобы клиент мог прочитать с фолловера/обсёрвера новое значение, которое ещё не доехало.

## Лог транзакций


### Найдите код, который выполняет сегментирование лога транзакций и преаллокацию сегментов
[Вот](https://github.com/apache/zookeeper/blob/fe940cdd8fb23ba09684cefb73233d570f4a20fa/zookeeper-server/src/main/java/org/apache/zookeeper/server/util/LogChopper.java#L44) и [вот](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/persistence/FilePadding.java#L75)

### Как выполняется сброс данных лога на диск?


### Найдите код, который выполняет оптимизацию group commits, т.е. батчит записи в лог транзакций
[Тут](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/persistence/FileTxnLog.java#L278) создаются всякие архивы над буферами над стримами над файлами, а [тут](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/persistence/FileTxnLog.java#L382) это всё флашится и фсинкается.

## Снимки состояния


### Какие данные составляют снимок состояния?
Дерево с его данными и метаданными, сессии с таймаутами, `zxid` последней обработанной транзакции ([отсюда](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/persistence/FileTxnSnapLog.java#L445) и глубже по стеку).

### Найдите код, который пишет снимок дерева. Как он синхронизирован с конкурирующими апдейтами?
[Тут](https://github.com/apache/zookeeper/blob/794adf11220151496a1ff877df3d2627719c9505/zookeeper-server/src/main/java/org/apache/zookeeper/server/DataTree.java#L1320) дерево обходится dfs'ом и сериализуется каждая нода. Каждая нода копируется под мьютексом (synchronized), но всё дерево пишется неатомарно, какие-то данные могут поменяться в процессе записи. Но если к такому побитому снепшоту применить записи из лога (начиная с той, `zxid` которой был записан в имя снепшота перед началом записи снепшота), то всё станет хорошо, т.к. зукипер специально для этого делает операции иденпотентными.

### Где и при каком условии запускается создание снапшота?
[Условие](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/SyncRequestProcessor.java#L143), [создание](https://github.com/apache/zookeeper/blob/11c07921c15e2fb7692375327b53f26a583b77ca/zookeeper-server/src/main/java/org/apache/zookeeper/server/SyncRequestProcessor.java#L182)
