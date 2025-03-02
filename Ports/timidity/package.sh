#!/usr/bin/env -S bash ../.port_include.sh
port='timidity'
useconfigure='true'
version='2.15.0'
files="https://netcologne.dl.sourceforge.net/project/timidity/TiMidity%2B%2B/TiMidity%2B%2B-${version}/TiMidity%2B%2B-${version}.tar.xz timidity-${version}.tar.xz 9eaf4fadb0e19eb8e35cd4ac16142d604c589e43d0e8798237333697e6381d39
https://www.quaddicted.com/files/idgames/sounds/eawpats.zip eawpats.zip.no_extract 19087fa4a40e25ec39a09cffcc9f775fc22d88bc971a7a9831e075cdae2ee1e3"
auth_type='sha256'
workdir="TiMidity++-${version}"
use_fresh_config_sub='true'
use_fresh_config_guess='true'
config_sub_paths=(
    'autoconf/config.sub'
)
config_guess_paths=(
    'autoconf/config.guess'
)
configopts=(
    '--without-x'
    'lib_cv___va_copy=no'
    'lib_cv_va_copy=no'
    'lib_cv_va_val_copy=no'
)

post_install() {
    # Unpack Eawpats
    eaw_pats_dir='/usr/local/share/eawpats'
    eaw_pats_host_dir="${SERENITY_INSTALL_ROOT}${eaw_pats_dir}"
    mkdir -p "${eaw_pats_host_dir}"
    run unzip -qo -d "${eaw_pats_host_dir}" '../eawpats.zip.no_extract'

    # Set up timidity.cfg
    timidity_cfg_path="${SERENITY_INSTALL_ROOT}/etc/timidity.cfg"
    mkdir -p "$(dirname ${timidity_cfg_path})"
    cp "${eaw_pats_host_dir}/timidity.cfg" "${timidity_cfg_path}"
    sed -i "s#^dir .*#dir ${eaw_pats_dir}#g" "${timidity_cfg_path}"
}
