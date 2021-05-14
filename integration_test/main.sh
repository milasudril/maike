#!/bin/bash

export install_dir=$(mktemp -d)
trap "rm -rf $install_dir" EXIT

bootsrap_from_version="2.0.0-beta-5" integration_test/bootstrap_from_release.sh
target_dir=__targets integration_test/build_new_version.sh
integration_test/can_generate_working_bootstrap_script.sh __targets/src/bin/maike2
integration_test/install_and_rebuild.sh
integration_test/install_and_rebuild_out_of_tree.sh