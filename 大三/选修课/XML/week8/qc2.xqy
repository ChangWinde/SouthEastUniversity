for $x in doc("dataset.xml")/figureSkating/mixedPair/team
where $x/ranking < 7 and $x/womanPlayer/nationality = 'USA'
order by $x/ranking
return $x