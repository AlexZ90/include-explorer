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

Запуск include-explorer необходимо выполнить 2 раза:

* Первый запуск для формирования файлов для анализа 
* Второй запуск для анализа файлов 

При первом запуске необходимо указать:

* опцию -f "путь к сгенерированному json-файлу c указанием имени" (обязательно наличие '/') (по умолчанию "./compile_commands.json"),
* опцию -p "префикс для компилятора" (опционально, по умолчанию пустая строка),
* формат: `include-explorer -f path/to/json/file/dir/json_file_name.json -p compiler_prefix`.
* пример  `include-explorer -f test/compile_commands.json -p /opt/arm-linux-gnueabihf/arm-linux-gnueabihf-`

При втором запуске необходимо указать:

* опцию -a "путь к файлу для анализа" (обязательно наличие '/') (по умолчанию необходмо использовать "./tmpMakefileResult.txt"),
* опцию -d "часть пути к файлам проекта" - часть пути к файлам компилируемого проекта, которая позволит определить, что файл находится в директории проекта. Если путь к файлу содержит указанную в этой опции подстроку, то он будет считаться в проекте.
* формат: `include-explorer -a path/to/file/to/analyze -d substring/of/project/dir/path`.
* пример: `include-explorer -a ./tmpMakefileResult.txt -d include-explorer/test`

Директория test содержит тестовый проект на С. Для тестового запуска необходимо в директории include-explorer выполнить команду `make prepare_test`, затем перейти в директорию test и выполнить команды:

* `include-explorer -f ./compile_commands.json`,
* `include-explorer -a ./tmpMakefileResult.txt -d include-explorer/test`

Поместить запуск теста в Makefile не удалось, так как при выполнении второй команды из Make-файла почему-то возникает ошибка, нужно вызывать команды вручную.

# Замечания
При запуске приложения в директории создаются два временных файла, которые можно удалить: tmpJson.json, tmpMakefile.

* `tmpJson.json` - файл json, содержащий команды компиляции, и предназначенный для парсинга и получения из него команд с помощью rapidjson. Является копией файла, передаваемого в опции -f, но отформатированный для парсинга с помощью rapidjson. Формируется при первом запуске.
* `tmpMakefile` - временный Make-файл для вызова команд компиляции с опцией -H. Формируется при первом запуске.
* `tmpMakefileResult.txt` - результат выполнения файла tmpMakefile. Формируется при первом запуске.
* `include-explorer-direct-includes.txt` - файл, содержащий команды компиляции файлов, в которых пристутствовала опция `-include`. Необходимо для определения файлов, которые были включены непосредственно в командах компиляции, так как они не показываются при использовании опции -H. Формируется при первом запуске.

* `include-explorer-result.txt` - результат анализа файла tmpMakefileResult.txt
* `include-explorer-result-short.txt` - результат анализа файла tmpMakefileResult.txt, но без полных путей
* `in_std_c.txt` - список файлов предположительно из стандартной библиотеки C
* `in_std_cxx.txt` - список файлов предположительно из стандартной библиотеки C++
* `not_project.txt` - список файлов предположительно не из проекта и не из стандартной библиотеки С/C++ (возможно, из другой библиотеки)
* `cp_source_files.sh` - скрипт для копирования копилируемых файлов в отдельную директорию compiled_sources (может использоваться, если необходимо посчитать строки кода и т.д.)


Для просмотра полного вывода нужно выполнить команду `make -f tmpMakefile`.