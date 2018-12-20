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
	* опцию -f "путь к сгенерированному json-файлу c указанием имени" (обязательно наличие '/') (по умолчанию "./compile_commands.json"),
	* опцию -p "префикс для компилятора" (опционально, по умолчанию пустая строка),
	* формат: `./include-explorer -f path/to/json/file/dir/json_file_name.json -p compiler_prefix`.

Директория test содержит тестовый проект на С. Для тестового запуска необходимо в директории include-explorer выполнить команду `make prepare_test`, а затем команду `./include-explorer -f test/compile_commands.json`.



# Замечания
При запуске приложения в директории создаются два временных файла, которые можно удалить: tmpJson.json, tmpMakefile.
Выводятся следующие уровни включения заголовочных файлов:
- Заголовочные файлы, включенные в команде gcc с помощью опции `-include`.
- Команды, содержащие опции `-include`.
- Результат выполнения команды -H компилятора для всех скомпилированных файлов.

Пример вывода:
```bash
alex@alex:~/Downloads/rapidjson-test/include-explorer$ ./include-explorer test/ compile_commands.json gcc
-include files list:
./include/foo3/foo3.h
./include/foo3/foo3.h
./include/foo3/foo3.h
./include/foo3/foo3.h

Includes that come directly from gcc command:
cd /home/alex/Downloads/git/include-explorer/test && gcc -c -DSTR="\"Hello World"\" -DERROR_PRAGMA_TEST -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/foo1.c -H -fsynta
x-only 
cd /home/alex/Downloads/git/include-explorer/test && gcc -c -DSTR="\"Hello World"\" -DERROR_PRAGMA_TEST -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/foo2.c -H -fsynta
x-only 
cd /home/alex/Downloads/git/include-explorer/test && gcc -c -DSTR="\"Hello World"\" -DERROR_PRAGMA_TEST -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/foo3.c -H -fsynta
x-only 
cd /home/alex/Downloads/git/include-explorer/test && gcc -c -DSTR="\"Hello World"\" -DERROR_PRAGMA_TEST -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/main.c -H -fsynta
x-only 



Other levels includes:
cd /home/alex/Downloads/git/include-explorer/test && gcc -c -DSTR="\"Hello World"\" -DERROR_PRAGMA_TEST -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/foo1.c -H -fsyntax-only 
x ././include/foo3/foo3.h.gch
. include/foo2/foo2.h
Multiple include guards may be useful for:
././include/foo3/foo3.h
/usr/include/wchar.h
/usr/include/x86_64-linux-gnu/bits/stdio_lim.h
/usr/include/x86_64-linux-gnu/bits/sys_errlist.h
/usr/include/x86_64-linux-gnu/bits/typesizes.h
/usr/include/x86_64-linux-gnu/gnu/stubs-64.h
/usr/include/x86_64-linux-gnu/gnu/stubs.h
include/foo2/foo2.h
cd /home/alex/Downloads/git/include-explorer/test && gcc -c -DSTR="\"Hello World"\" -DERROR_PRAGMA_TEST -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/foo2.c -H -fsyntax-only 
x ././include/foo3/foo3.h.gch
Multiple include guards may be useful for:
././include/foo3/foo3.h
/usr/include/wchar.h
/usr/include/x86_64-linux-gnu/bits/stdio_lim.h
/usr/include/x86_64-linux-gnu/bits/sys_errlist.h
/usr/include/x86_64-linux-gnu/bits/typesizes.h
/usr/include/x86_64-linux-gnu/gnu/stubs-64.h
/usr/include/x86_64-linux-gnu/gnu/stubs.h
cd /home/alex/Downloads/git/include-explorer/test && gcc -c -DSTR="\"Hello World"\" -DERROR_PRAGMA_TEST -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/foo3.c -H -fsyntax-only 
x ././include/foo3/foo3.h.gch
Multiple include guards may be useful for:
././include/foo3/foo3.h
/usr/include/wchar.h
/usr/include/x86_64-linux-gnu/bits/stdio_lim.h
/usr/include/x86_64-linux-gnu/bits/sys_errlist.h
/usr/include/x86_64-linux-gnu/bits/typesizes.h
/usr/include/x86_64-linux-gnu/gnu/stubs-64.h
/usr/include/x86_64-linux-gnu/gnu/stubs.h
cd /home/alex/Downloads/git/include-explorer/test && gcc -c -DSTR="\"Hello World"\" -DERROR_PRAGMA_TEST -I include/foo1/ -I include/foo2/ -include ./include/foo3/foo3.h /home/alex/Downloads/git/include-explorer/test/src/main.c -H -fsyntax-only 
x ././include/foo3/foo3.h.gch
. include/foo1/foo1.h
.. include/foo2/foo2.h
Multiple include guards may be useful for:
././include/foo3/foo3.h
/usr/include/wchar.h
/usr/include/x86_64-linux-gnu/bits/stdio_lim.h
/usr/include/x86_64-linux-gnu/bits/sys_errlist.h
/usr/include/x86_64-linux-gnu/bits/typesizes.h
/usr/include/x86_64-linux-gnu/gnu/stubs-64.h
/usr/include/x86_64-linux-gnu/gnu/stubs.h
include/foo1/foo1.h
include/foo2/foo2.h

Finish.


```

Для просмотра полного вывода нужно выполнить команду `make -f tmpMakefile`.
