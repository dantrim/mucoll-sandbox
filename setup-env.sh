#!/bin/sh

image="infnpd/mucoll-ilc-framework"
image_tag="1.5-centos8"
default_config_dirname="mucoll_config"
default_config_path="${PWD}/../${default_config_dirname}"
default_workdir="${PWD}"

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
    echo "  -w|--workdir        Path to your run directory"
    echo "                       (default: PWD=${default_workdir})"
    echo "  -h|--help           Print this help message"
    echo "---------------------------------------------------------"
}

function main {

    config_path=${default_config_path}
    workdir_path=${default_workdir}

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
            -w)
                workdir_path=${2}
                shift
                ;;
            --workdir)
                workdir_path=${2}
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

    if [ ! -d ${workdir_path} ]; then
        echo "ERROR: Could not find workdir directory \"${workdir_path}\""
        return 1
    fi

    # expand any relative paths (we know that the relative paths exist from the checks above
    config_path=$(cd ${config_path}; pwd)
    workdir_path=$(cd ${workdir_path}; pwd)

    cmd="docker run --rm -ti -v ${workdir_path}:/workdir -v ${config_path}:/mucoll_config/ ${image}:${image_tag}"
    echo "Running: ${cmd}"
    $cmd
}

#_______
main $*
