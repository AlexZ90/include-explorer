# Описание
Приложение include-explorer предназначено для вывода списка заголовочных файлов, необходимых для компиляции программ с помощью gcc.
Для вывода заголовочных файлов используется опция -H gcc, пакеты Bear и rapidjson.

# Зависимости
Для сборки и запуска приложения необходимо наличие следующих пакетов:
* Rapidjson:
	* Репозиторий rapidjson: https://github.com/Tencent/rapidjson.git
	* Проверенный коммит: 2bbd33b33217ff4a73434ebf10cdac41e2ef5e34
* Bear (Build EAR):
	* Репозиторий: https://github.com/rizsotto/Bear.git

# Сборка

Для сборки приложения необходимо:
* В Make-файле изменить путь к заголовочным файлам rapidjson (переменная RAPIDJSON_DIR).
* Запустить сборку, выполнив команду make.

# Запуск
Для запуска приложения необходимо выполнить следующие действия:
* Собрать из исходников и установить Bear (по инструкции из репозитория).
* Перейти в директорию некоторого проекта, в котором планируется запускать make, и выполнить команду `bear make`. В результате в директории проекта появится файл `compile_commands.json` с записанными командами gcc.
* Запустить include-explorer, указав:
	* путь к сгенерированному json-файлу,
	* имя сгенерированного json-файла,
	* команда компилятора,
	* формат: `./include-explorer path/to/json/file/dir/ json_file_name.json compiler_command`.

Директория test содержит тестовый проект на С. Для тестового запуска необходимо в директории include-explorer выполнить команду `make prepare_test`, а затем команду `./include-explorer test/ compile_commands.json gcc`.



# Замечания
При запуске приложения в директории создаются два временных файла, которые можно удалить: tmpJson.json, tmpMakefile.
Выводятся следующие уровни включения заголовочных файлов:
- Заголовочные файлы, включенные в команде gcc с помощью опции `-include`.
- Заголовочные файлы первого уровня - файлы, которые включаются в C-файлы, которые компилируются с помощью команды gcc.
- Заголовочные файлы остальных уровней вложения.

Пример вывода:
```bash
alex@alex:~/Downloads/rapidjson-test/include-explorer$ ./include-explorer test/ compile_commands.json gcc

Includes that come directly from gcc command:
cd test/ && gcc -c -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/rapidjson-test/include-explorer/test/src/foo1.c -H 
cd test/ && gcc -c -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/rapidjson-test/include-explorer/test/src/foo2.c -H 
cd test/ && gcc -c -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/rapidjson-test/include-explorer/test/src/foo3.c -H 
cd test/ && gcc -c -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/rapidjson-test/include-explorer/test/src/main.c -H 

First level includes (includes that are in source file compiled with gcc):
. include/foo1/foo1.h
. include/foo2/foo2.h

Other levels includes:
.. include/foo2/foo2.h

Finish. To see full output run 'make -f tmpMakefile'.

```

Для просмотра полного вывода нужно выполнить команду `make -f tmpMakefile`.
