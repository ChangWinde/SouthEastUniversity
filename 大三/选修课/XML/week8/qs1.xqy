(:~ 筛选排名前2的单人滑雪女参赛者的姓名与年纪 ~:)
for $x in doc("dataset.xml")/figureSkating/singleSki/contestant
order by $x/ranking
return <player>{$x/ranking} {$x/name} {$x/age}</player>
(:~ 由于一条记录刚好占五行，所以输出重定向时直接过管道 head即可，head默认读前10行 ~:)
(:~ basex qs1.xqy | head > qs1.xml ~:)
