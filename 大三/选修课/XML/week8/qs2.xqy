for $x in doc("dataset.xml")/figureSkating/mixedPair/team/manPlayer
order by $x/age
return <player> {$x/name} {$x/age}</player>