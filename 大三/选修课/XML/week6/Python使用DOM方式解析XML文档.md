## Python使用DOM方式解析XML文档

学号：71119104
姓名：包亦成

DOM(Document Object Model)文档对象模型是HTML和XML文档的编程接口。它提供了对文档的结构化的表述，并定义了一种方式可以使从程序中对该结构进行访问，从而改变文档的结构，样式和内容。DOM 将文档解析为一个由节点和对象（包含属性和方法的对象）组成的结构集合。简言之，它会将web页面和脚本或程序语言连接起来。

而XML也可以用DOM的方式把数据在内存中解析成一棵树，以此来操作XML。即将文档读取到内存中操作，然后导出到磁盘中，优点是操作方便，适合较小的文件，如果文档很大的话，那么将会很占内存。

### 实现方法

首先是测试使用的XML文档（test.xml）

~~~xml
<?xml version="1.0" encoding="utf-8"?>
<table>
  <person id="person1">
     <name>Peter</name>
     <age>21</age>
     <sex>Male</sex>
  </person>
  <person id="person2">
     <name>Kitty</name>
     <age>21</age>
     <sex>Female</sex>
  </person>
</table>
~~~

然后通过python来操作DOM提供的API来读取XML文档中的元素，

~~~python
from xml.dom.minidom import parse
 
# 读取XML文档
dom = parse('test.xml')
# 获取文档元素对象
data = dom.documentElement
# 获取 person
persons = data.getElementsByTagName('person')
for per in persons:
    # 获取标签属性值
    per_name = per.getAttribute('name')
    per_id = stu.getAttribute('id')
    # 获取标签中内容
    name = stu.getElementsByTagName('name')[0].childNodes[0].nodeValue
    age = stu.getElementsByTagName('age')[0].childNodes[0].nodeValue
    gender = stu.getElementsByTagName('sex')[0].childNodes[0].nodeValue
    print('per_id:', per_id)
    print('name:', name, ', age:', age, ', sex:',sex)
~~~

解析结果：

~~~
per_id: stu1
name: Peter , age: 22 , sex: Male
per_id: stu2
name: Kitty , age: 21 , sex: Female
~~~

从输出结果可以看出DOM已经正确解析出了XML文档的指定元素。

