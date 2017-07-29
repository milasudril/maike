#@	{
#@	"targets":
#@		[{
#@		"name":"options.troff","dependencies":
#@			[{"ref":"ronn","rel":"tool"}
#@			,{"ref":"sed","rel":"tool"}
#@			,{"ref":"../maike","rel":"misc"}]
#@		}]
#@	}

abort()
	{
	exit -1
	}
trap 'abort' 0
set -eo pipefail

dir_target="$1"
in_dir="$2"

"$dir_target"/maike --help \
	| sed 's/\(^--[a-z][a-z\-]*\)\(\[\?[ =]\?\)\([a-z ]*\)/`\1`\2*\3*/g' \
	| sed 's/\*\*//g' \
	| ronn \
	| sed 's/^ \.SH/.SS/'> "$dir_target"/"$in_dir"/options.troff

trap : 0
