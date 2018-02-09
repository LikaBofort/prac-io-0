Подготовка данных    
----------------------------------------

Используем данные, доступные по ссылке:  
<https://console.cloud.google.com/storage/browser/artem-pyanykh-cmc-prac-task3-seed17/out/input/>   

Для работы с Google Cloud используем пакет консольных инструментов gcloud. Устанавливаем gsutil как часть Google Cloud SDK.
Приведен алгоритм для Ubuntu. Для других ОС - <https://cloud.google.com/storage/docs/gsutil_install>

Работаем с коммандной строкой.

Настройки окружения для корректной загрузки:

    export CLOUD_SDK_REPO="cloud-sdk-$(lsb_release -c -s)"

Настройки источника пакета. Вы можете использовать 'https' вместо 'http' на этом шаге:

    echo "deb http://packages.cloud.google.com/apt $CLOUD_SDK_REPO main" | sudo tee -a /etc/apt/sources.list.d/google-cloud-sdk.list

Импортируем открытый ключ Google Cloud::

    curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -
    
Обновляем и устанавливаем Cloud SDK:

    sudo apt-get update && sudo apt-get install google-cloud-sdk

Устанавливаем дополнительные пакеты для Python:

    sudo apt-get install google-cloud-sdk-app-engine-python
    
Если работаем в браузере, не загружающим автоматически URL, запускаем gcloud init, чтобы начать, так:

    gcloud init --console-only
   
Иначе:

    gcloud init
    
Видим сообщение:

>Welcome! This command will take you through the configuration of gcloud.   
>Your current configuration has been set to: [default] 
>To continue, you must login. Would you like to login (Y/n)?

Отвечаем "Y" для авторизации. Далее: 

>Your browser has been opened to visit:  
>https://accounts.google.com/o/oauth2/auth?redirect_uri=http%3A%2F%2F...

Если браузер не загружает автоматически URL:

>Go to the following link in your browser:  
>https://accounts.google.com/o/oauth2/auth?redirect_uri=urn%3Aietf%3Awg%3A...  
>Enter verification code:

Если окно браузера открылось автоматически, нажимаем "принять". 
После этого код подтверждения автоматически отправляется в командную строку.

Если работаем на удаленном компьютере или используем флаг --console only, копируем код подтверждения из URL-адреса
и вставляем его в командную строку терминала после

>Enter verification code:

Выбераем конфигурацию и проект по умолчанию для этой конфигурации.

После настройки учетных данных gcloud предложит проект по умолчанию для этой конфигурации и предоставит список доступных проектов. 
Выбираем идентификатор проекта из списка. Готово.

Теперь с помощью команды `cp` копируем файлы из облака на компьютер:

    gsutil cp gs://artem-pyanykh-cmc-prac-task3-seed17/out/input/*.csv ./input


