INPUT_PPM="input.ppm"
OUTPUT_DIR="Output"
SHOULD_DEPLOY=false
PAGE_STITCH_COUNT="30"
POSITIONAL=()
while [[ $# -gt 0 ]]
do
	key="$1"
	case $key in
		-i)
		INPUT_PPM="$2"
		shift
		shift
		;;
		-c)
		PAGE_STITCH_COUNT="$2"
		shift
		shift
		;;	
		deploy | --deploy)
		SHOULD_DEPLOY=true
		shift
		shift
		;;
		*)
		POSITIONAL+=("$1")
		shift
		;;
	esac
done

if [ "$SHOULD_DEPLOY" = true ] ; then
	make clean build
fi

if [ ! -d "$OUTPUT_DIR" ] ; then
	mkdir "$OUTPUT_DIR"
fi

rm Output/*

./CSPT.exe "${INPUT_PPM}" "${PAGE_STITCH_COUNT}" "${OUTPUT_DIR}"
