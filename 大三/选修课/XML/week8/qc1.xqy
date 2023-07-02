for $x in doc("dataset.xml")/figureSkating/singleSki/contestant
where $x/age < 20 and $x/ranking < 7
order by $x/ranking
return $x