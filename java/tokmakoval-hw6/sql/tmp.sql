# Write your MySQL query statement below
select Day, sum(case when Status = 'completed' then 0 else 1 end) / count(*)
from Trips join Users on Client_Id = User_Id or Client_Id = Driver_id
where Request_at between '2013-10-01' and '2013-10-03'
group by Day