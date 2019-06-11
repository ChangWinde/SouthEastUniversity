from queue import Queue
import xlrd,os,time,argparse
# 判断一个字符是否是一个数字
def isDigit(charactor):
    return charactor.isdigit()

# 判断一个字符是否是一个字母
def isLetter(charactor):
    return charactor.isalpha()

# 判断一个字符串是不是一个关键字
def isBasicWord(string):
    return string in basicWordTable

# 判断一个字符是不是一个分隔符
def isSeparator(charactor):
    return charactor in separatorTable

# 判断一个字符串是不是一个操作符
def isOperator(string):
    return string in operatorTable

# 判断一个字符串是不是可能是一个操作符
def isLikeOperator(string):
    return string in "".join(operatorTable)

# 是否空白符
def isSpace(charactor):
    return charactor in " \n\r\t"

#===============================================
# 基本关键字表 0-61
basicWordTable = list()
basicWordTable = [
    "include","define","auto","bool","break","case","catch","char","class",
    "const","const_cast","continue","default","delete","do","double",
    "dynamic_cast","else","enum","explicit","extern","false","float","for",
    "friend","goto","if","inline","int","long","mutable","namespace","new",
    "operator","private","protected","public","register","reinterpret_cast",
    "return","short","signed","sizeof","static","static_cast","struct",
    "switch","template","this","throw","true","try","typedef","typeid",
    "typename","union","unsigned","using","virtual","void","volatile","while" 
]

# 分隔符表 65-79
separatorTable = list()
separatorTable = [",",";",".","(",")","[","]","{","}","#"]

# 操作运算符表 80-96
operatorTable = list()
operatorTable = ["+", "-","++","--", "*", "/", "<", "<=", ">", ">=", "=", "==","!=","<<",">>","&&","||"]

# 标识符表 100
identitierTable = list()
# 字符常量表 101
charTable = list()
# 字符串常量表 102
stringTable = list()
# 整数常量表 103
integerTable = list()
# 浮点数常量表 104
floatTable = list()

# 各种表的起始偏移
# 分隔符表
separatorOffset = 65
# 操作运算符表
operatorOffset = 80
# 标识符表 
identitierOffset = 500
# 字符常常量表
charId = 101
# 字符串常量表
stringId = 102
# 整数常量表
integerId = 103
# 浮点数常量表
floatId = 104
# 注释
commentId = 200
# 错误代码
errorId = 404
# 文件标记
mark = str(time.time())[-6:-1]
#============================================

# 扫描器
def Scanner(text,pointer):
    # 准备的工作
    i = pointer
    end = len(text)-1
    buffer = ""
    classNumber = -1
    # 开始扫描整个程序 
    while i<= end and isSpace(text[i]):
        i += 1
    # 终止条件
    if i > end:
        return  buffer,classNumber,-1
    # 开头是数字
    if isDigit(text[i]):
        while i <= end and (isDigit(text[i]) or text[i] == '.'):
            buffer += text[i]
            i += 1
        if buffer.count(".")>1:
            classNumber = errorId
            buffer = "bad digit"
        if i<=end and isLetter(text[i]):
            classNumber = errorId
            buffer = "bad digit(letter can not appear after digital variable)"
        elif "." in buffer:
            if buffer not in floatTable:
                floatTable.append(buffer)
            classNumber = floatId
        else:
            if buffer not in integerTable:
                integerTable.append(buffer)
            classNumber = integerId
    # 开头是字母
    elif isLetter(text[i]):
        while i<=end and (isDigit(text[i]) or isLetter(text[i])):
            buffer += text[i]
            i += 1
        # 是否是一个关键字
        if isBasicWord(buffer):
            classNumber = basicWordTable.index(buffer)
        # 是一个标识符
        else:
            if buffer not in identitierTable:
                identitierTable.append(buffer)
            classNumber = identitierOffset
    # 开头是分隔符
    elif isSeparator(text[i]):
        buffer += text[i]
        classNumber = separatorTable.index(text[i])+separatorOffset
        i += 1
    # 字符
    elif text[i] == '\'':#多位字符将出现错误
        if i+2 > end:
            if i+1<=end:
                i += 1
            classNumber = errorId
            buffer = "Wrong statement of char"
        elif text[i+2] == '\'':
            buffer = text[i:i+3]
            if buffer not in charTable:
                charTable.append(buffer)
            classNumber = charId
            i += 3
        else:
            i += text[i:].index('\n')
            classNumber = errorId
            buffer = "Wrong statement of char" 
    # 字符串
    elif text[i] == "\"":
        p = i+text[i:].index("\n")
        if "\"" not in text[i+1:p]:
            i = p+1
            classNumber = errorId
            buffer = "Wrong statement of string"
        else:
            buffer += "\""
            i += 1
            while i<=end and (text[i-1] == "\\" or text[i] != "\""):
                buffer += text[i]
                i += 1
            buffer += "\""
            i += 1
            classNumber = stringId
        if buffer not in stringTable:
            stringTable.append(buffer)
    # 注释部分
    elif text[i] == '/':
        buffer += '/'
        i += 1
        if i <=end and text[i] == '/':
            while i<=end and text[i] != '\n':
                buffer += text[i]
                i += 1
            classNumber = commentId
        elif i<=end and text[i] == '*':
            while i<=end and (text[i-1] != '*' or text[i] != '/'):
                buffer += text[i]
                i+= 1
            if i<=end:
                buffer += '/'
                i += 1
                buffer = buffer.replace('\n','')
                classNumber = commentId
            else:
                classNumber = errorId
                buffer = "Comment symbol is incomplete（Missing the second half）"
        else:
            classNumber = operatorTable.index(text[i-1])+operatorOffset  
    # 开头是操作符
    elif isLikeOperator(text[i]):
        buffer += text[i]
        if i+1<=end and isOperator(buffer+text[i+1]):
            i += 1
            buffer += text[i]
            classNumber = operatorTable.index(buffer)+operatorOffset
        elif isOperator(buffer):
            classNumber = operatorTable.index(text[i])+operatorOffset
        else:
            classNumber = errorId
            buffer = "bad identitier"
        i += 1
    # 无法正确识别
    else:
        i += 1
        classNumber = errorId
        buffer = "the identitier can not be identified"
    
    return buffer,classNumber,i

# 读取源文件
def readFile(path):
    with open(path,encoding = 'utf-8')as f:
        text = f.read()
    return text

#  词法分析
def analyzeWord(fileName):
    text = readFile(fileName)
    i = 0
    line = 0
    q = Queue()
    flag = False
    buffer=""
    while i>=0:
        context,classNum,i= Scanner(text,i)
        if(i>0):
            if context in ['#','include','<','using','namespace']:
                flag = True
                if buffer == '#include' or buffer != '<':
                    buffer += context
            else:
                if flag:
                    q.put(buffer)
                    flag =False
                    buffer = ""
                if classNum == 500:
                    context = "Id"
                elif classNum == 103 or classNum == 104:
                    context = "NUM"
                q.put(context)
    return q

# excel的操作
def getProduct(reader,sp):
    data = xlrd.open_workbook('./PPT.xlsx')
    ppt = data.sheets()[0]
    product = data.sheets()[1]
    row = ppt.row_values(0)
    col = ppt.col_values(0)
    i = row.index(reader)
    j = col.index(sp)
    product_index = int(ppt.cell(j,i).value)
    return product.cell(product_index-1,1).value

# 写回结果
def writeResult(fileName,context):
    if(not os.path.exists('./derivation')):
        os.mkdir('./derivation')
    with open('./derivation/{}-{}.txt'.format(fileName,mark),mode="a+",encoding='utf-8')as f:
        f.write(context)

product=""
stackPointer=""
# 语法分析
def syntaxAnalyse(stack,queue,fileName):
    isNext = True
    while not (queue.empty() and stackPointer == "#"):
        if isNext and not queue.empty():
            reader = queue.get()
            isNext = False
        stackPointer = stack.pop()
        if stackPointer == 'ε':
            stackPointer = stack.pop()
        print("reader:{}\nstackPointer:{}".format(reader,stackPointer))
        writeResult(fileName,"reader:{}\nstackPointer:{}\n".format(reader,stackPointer))
        if stackPointer == reader:
            isNext = True
            print("success!\nstack:{}\n".format(stack))
            writeResult(fileName,"success!\nstack:{}\n\n".format(stack))
            continue
        product = getProduct(reader,stackPointer)
        print("product:{}".format(product))
        writeResult(fileName,"product:{}\n".format(product))
        product = str.split(product[product.index('->')+2:].strip(),' ')
        for i in reversed(product):
            stack.append(i)
        print("stack:{}\n".format(stack))
        writeResult(fileName,"stack:{}\n\n".format(stack))
    print("Syntax analyse is completed!")

# 参数解析
def argParse():
    parser = argparse.ArgumentParser()
    # Add argument
    parser.add_argument('--file','-f', type=str, help='path of the file to analyze')
    return parser.parse_args()

if __name__ == "__main__":
    args = argParse()
    q = analyzeWord(args.file)
    fileName = os.path.basename(args.file)
    q.put('#')
    stack = ['#','P']
    fileName = os.path.basename(fileName)
    fileName = fileName[:fileName.index('.')]
    syntaxAnalyse(stack,q,fileName)