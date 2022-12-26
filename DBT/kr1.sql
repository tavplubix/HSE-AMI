-- 1
select ID, cast(replace(S1,S2, S3) as varchar(100)) as Res1 from table1;


-- 2
select ID, coalesce(Table2.S3, Table3.S3) as S3
from          Table1
    left join Table2 on Table1.S1 = Table2.S1ID
    left join Table3 on Table1.S1 = Table3.S1ID
where
      Table2.S3 is not null
   or Table3.S3 is not null;


-- 3
select TradeID, c.Currency as Currency, cast(sum(Trades.Value * coalesce(Coeff, 1)) as numeric(38,2)) as Total
from Trades join lateral
    (
        -- для каждой строки находим, в какую валюту переводить
        select Currency from
                          (select distinct Currency from Trades as t where t.TradeID=Trades.TradeID) as x
        left join Rates on Currency = FromCurrency and ToCurrency = 'USD'
        order by Coeff asc nulls last limit 1
        -- Из условия не понятно, надо ли приводить к USD, если в rates нет пары (USD, USD),
        -- а у остальных валют курс больше 1. Пусть будет валюта с наименьшим курсом, кроме USD/USD.
    ) as c on true
left join Rates on Rates.FromCurrency = Trades.Currency and Rates.ToCurrency = c.Currency
group by TradeID, c.Currency;



