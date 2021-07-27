#!/bin/sh

image="infnpd/mucoll-ilc-framework"
image_tag="1.0-centos8"
default_config_dirname="mucoll_config"
default_config_path="${PWD}/../${default_config_dirname}"

function print_usage {
    echo "---------------------------------------------------------"
    echo " Setup the Docker environment for running muon collider software"
    echo ""
    echo " Usage:"
    echo "   $ source /path/to/setup-mucoll.sh [OPTIONS]"
    echo ""
    echo " Options:"
    echo "  -c|--config-path    Path to muon collider steering files"
    echo "                       (default: ${default_config_path})"
    echo "  -h|--help           Print this help message"
    echo "---------------------------------------------------------"
}

function main {

    config_path=${default_config_path}

    while test $# -gt 0
    do
        case $1 in 
            -c)
                config_path=${2}
                shift
                ;;
            --config-path)
                config_path=${2}
                shift
                ;;
            -h)
                print_usage
                return 0
                ;;
            --help)
                print_usage
                return 0
                ;;
        esac
        shift
    done

    if [ ! -d ${config_path} ]; then
        echo "ERROR: Could not find config directory \"${config_path}\""
        return 1
    fi

    cmd="docker run --rm -ti -v ${PWD}:/workdir -v ${PWD}/${config_path}:/mucoll_config/ ${image}:${image_tag}"
    echo "Running: ${cmd}"
    $cmd
}

#_______
main $*
