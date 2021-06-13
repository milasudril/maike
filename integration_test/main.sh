#!/bin/bash

set -e

export install_dir=$(mktemp -d)
trap "rm -rf $install_dir" EXIT

bootsrap_from_version="2.0.0-beta-9" integration_test/bootstrap_from_release.sh

target_dir=__targets integration_test/build_new_version.sh
exit_status=0
integration_test/can_generate_working_bootstrap_script.sh __targets/core/bin/maike2 || exit_status=$?
integration_test/install_and_rebuild.sh || exit_status=$?
integration_test/install_and_rebuild_out_of_tree.sh || exit_status=$?
integration_test/install_and_rebuild_out_of_tree_rel_path.sh || exit_status=$?
integration_test/install_and_rebuild_out_of_tree_from_subpath.sh || exit_status=$?
integration_test/check_release_tarball.sh || exit_status=$?

exit $exit_status