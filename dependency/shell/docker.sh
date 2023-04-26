
while getopts ':cr' 'OPT'; do
    case ${OPT} in
        'c') #传入cpp文件并编译
            #echo v"$VERSION"
            cd /usr/local/PlayerCode/CAPI/cpp
            ./compile.sh
            exit
            ;;
        'r') #运行
            #NO_PROMPT='true'
            cd /usr/local
            ./run.sh
            ;;
        'l') 

            ;;
        'i')
            INSTALL_VERSION="${OPTARG}"
            ;;
    esac
done