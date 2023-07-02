for $x in doc("books.xml")/books/book
where $x/price>30
return $x/title