#!/bin/bash

#
# Essentials

SCRIPT_NAME=$(basename $0)

TEXT_STYLE_NORMAL='\e[0m'
TEXT_STYLE_ERROR='\e[0;31m'

ERROR_STYLE_BEGIN=''
ERROR_STYLE_END=''
if [ -t 1 ] && [ $(tput colors) \> 0 ]
then
	ERROR_STYLE_BEGIN="$TEXT_STYLE_ERROR"
	ERROR_STYLE_END="$TEXT_STYLE_NORMAL"
fi

printf-error()
{
	printf "$ERROR_STYLE_BEGIN$SCRIPT_NAME error:$ERROR_STYLE_END " >&2
	printf "$@" >&2
	printf '\n\n' >&2
}


#
# Optionals

OUTPUT_DIRECTORY_SUFFIX='amalgamated'
DEFAULT_INPUT_HEADER_EXTENSIONS=(h hh hpp hxx h++ tpp txx tpl ii ixx ipp inl)
DEFAULT_INPUT_SOURCE_EXTENSIONS=(c cc cpp cxx c++)
DEFAULT_OUTPUT_HEADER_EXTENSION='h'
DEFAULT_OUTPUT_SOURCE_EXTENSION='cpp'

HELP_MESSAGE=\
"\e[1mUSAGE\e[0m
  amalgamate [options]                Amalgamate everything it can find in the current directory
  amalgamate [options] -- <files...>  Amalgamate specified files and their included files

\e[1mOPTIONS:\e[0m
  -I <header-path>      Path where header files can be found [default: current working directory's parent]
  -e <ext,ens,ions...>  Input header extensions [default: '$DEFAULT_INPUT_HEADER_EXTENSIONS'].
  -s <ext,ens,ions...>  Input source extensions [default: '$DEFAULT_INPUT_SOURCE_EXTENSIONS'].
  -o <output-dir-path>  Output directory [default: '../'].
  -H                    Amalgamate all files to single header file.
  -S                    Amalgamate all files to single source file.
  -n <base-name>        Base name for output files [default: parent directory name].
  -x <hpp,cpp>          Extensions for output header and source files [default: inferred from input files; if cannot, then '$DEFAULT_OUTPUT_HEADER_EXTENSION','$DEFAULT_OUTPUT_SOURCE_EXTENSION'].
  -g                    Group unexpanded headers on top.
  -E <count>            Do not allow more than <count> consecutive empty lines.
  -G <id> | -G ''       Use include guard in header instead of '#pragma once'. Use '<id>' for '#define' directive or generate it from header name if empty string was passed.
  -t                    Trim trailing whitespace.
  -a                    Insert annotations.
  -v                    Verbose mode.
  -h                    Show this message.

\e[1mMore information at \e[mhttps://github.com/shrpnsld/amalgamate\e[0m
"

HEADER_STRING_TEMPLATE='%s// ============================================================================\n%s// == %s ==\n%s// =='
FOOTER_STRING_TEMPLATE='%s// ==\n%s// == %s ==\n%s// ============================================================================'

HEADER_SEARCH_PATH=$(dirname $(pwd))
HEADER_EXTENSIONS=("${DEFAULT_INPUT_HEADER_EXTENSIONS[@]}")
SOURCE_EXTENSIONS=("${DEFAULT_INPUT_SOURCE_EXTENSIONS[@]}")
OUTPUT_DIR_PATH="$(pwd)/.."
HEADER_ONLY=false
SOURCE_ONLY=false
NAME=$(basename $(pwd))
OUTPUT_HEADER_EXTENSION=-1
OUTPUT_SOURCE_EXTENSION=-1
EMPTY_LINE_COUNT_LIMIT=''
TRIM_TRAILING_WHITESPACE=false
INCLUDE_GUARD_ID=''

MAKE_INCLUDE_GUARD_HEADER_FUNCTION=do-make-include-guard-pragma-once
MAKE_INCLUDE_GUARD_FOOTER_FUNCTION=:
FILE_PRINTF_ANNOTATION_FUNCTION=:
FILE_PRINT_HEADER_ANNOTATION_FUNCTION=:
FILE_PRINT_FOOTER_ANNOTATION_FUNCTION=:
INDENT_SHIFT_FUNCTION=:
PRINTF_VERBOSE_FUNCTION=:
PROCESS_STANDARD_HEADER_FUNCTION=file-printf
PREPEND_WITH_STANDARD_INCLUDES_FUNCTION=:


#
# Functions

PARAM_TYPE_OPTION=1
PARAM_TYPE_DELIMITER=2
PARAM_TYPE_BAD=255

check-parameter()
{
	local string=$1

	if [ "${string:0:3}" == '--' ] && [ "${#string}" == 2 ]
	then
		return $PARAM_TYPE_DELIMITER
	fi

	if [ "${string:0:1}" == '-' ]
	then
		printf '%s\n' "${string#-}"
		return $PARAM_TYPE_OPTION
	fi

	return $PARAM_TYPE_BAD
}


PARSE_OPTION_UNKNOWN_OPTION=255

parse-option()
{
	local option=$1
	local value=$2

	case "$option" in
		I)
			HEADER_SEARCH_PATH="$value"
			return 1
			;;

		e)
			HEADER_EXTENSIONS=($(split-string "$value" ','))
			return 1
			;;

		s)
			SOURCE_EXTENSIONS=($(split-string "$value" ','))
			return 1
			;;

		o)
			OUTPUT_DIR_PATH="$value"
			return 1
			;;

		H)
			HEADER_ONLY=true
			;;

		S)
			SOURCE_ONLY=true
			;;

		n)
			NAME="$value"
			return 1
			;;

		x)
			OUTPUT_HEADER_EXTENSION="${value%,*}"
			OUTPUT_SOURCE_EXTENSION="${value#*,}"
			return 1
			;;

		g)
			PROCESS_STANDARD_HEADER_FUNCTION=add-standard-include-line
			PREPEND_WITH_STANDARD_INCLUDES_FUNCTION=do-prepend-with-standard-includes
			;;

		E)
			EMPTY_LINE_COUNT_LIMIT="$value"
			return 1
			;;

		G)
			INCLUDE_GUARD_ID="$value"
			MAKE_INCLUDE_GUARD_HEADER_FUNCTION=do-make-include-guard-ifdef
			MAKE_INCLUDE_GUARD_FOOTER_FUNCTION=do-make-include-guard-endif
			return 1
			;;

		t)
			TRIM_TRAILING_WHITESPACE=true
			;;

		a)
			FILE_PRINTF_ANNOTATION_FUNCTION=file-printf
			FILE_PRINT_HEADER_ANNOTATION_FUNCTION=do-file-print-header-annotation
			FILE_PRINT_FOOTER_ANNOTATION_FUNCTION=do-file-print-footer-annotation
			INDENT_SHIFT_FUNCTION=do-indent-shift
			;;

		v)
			PRINTF_VERBOSE_FUNCTION=printf
			;;

		h)
			printf "$HELP_MESSAGE"
			exit
			;;

		*)
			return $PARSE_OPTION_UNKNOWN_OPTION
			;;
	esac
}


validate-options()
{
	if [ ! -d "$HEADER_SEARCH_PATH" ]
	then
		printf-error 'bad header search path "%s"' "$HEADER_SEARCH_PATH"
		return 1
	fi

	HEADER_SEARCH_PATH=$(expand-dir-path "$HEADER_SEARCH_PATH")


	if [ ! -d "$OUTPUT_DIR_PATH" ]
	then
		printf-error 'bad output directory path "%s"' "$OUTPUT_DIR_PATH"
		return 1
	fi

	OUTPUT_DIR_PATH=$(expand-dir-path "$OUTPUT_DIR_PATH")


	if [ "${#HEADER_EXTENSIONS[@]}" == 0 ]
	then
		printf-error 'input header extensions is empty'
		return 1
	fi

	if [ "${#SOURCE_EXTENSIONS[@]}" == 0 ]
	then
		printf-error 'input source extensions is empty'
		return 1
	fi


	if [ -z "$OUTPUT_HEADER_EXTENSION" ]
	then
		printf-error 'output header extension is empty'
		return 1
	fi

	if [ -z "$OUTPUT_SOURCE_EXTENSION" ]
	then
		printf-error 'output source extension is empty'
		return 1
	fi

	if [ "$OUTPUT_HEADER_EXTENSION" != -1 ] && [ "$OUTPUT_SOURCE_EXTENSION" != -1 ] && [ "$OUTPUT_HEADER_EXTENSION" == "$OUTPUT_SOURCE_EXTENSION" ]
	then
		printf-error 'extensions for output files cannot be the same'
		return 1
	fi

	if [ -n "$EMPTY_LINE_COUNT_LIMIT" ] && ! [[ $EMPTY_LINE_COUNT_LIMIT =~ ^([0-9]|[1-9][0-9]+)$ ]]
	then
		printf-error 'empty line count is not a positive integer'
		return 1
	fi
}


parse-parameters()
{
	local parameters=("$@")

	local parameter_index
	for (( parameter_index = 0; parameter_index < ${#parameters[@]}; ++parameter_index ))
	do
		local parameter
		parameter=$(check-parameter "${parameters[$parameter_index]}")
		local parameter_type=$?
		case $parameter_type in
			$PARAM_TYPE_OPTION)
				local value="${parameters[$parameter_index + 1]}"
				local option_index
				for (( option_index = 0; option_index < ${#parameter}; ++option_index ))
				do
					local option="${parameter:$option_index:1}"
					parse-option "$option" "$value"
					local retval=$?
					case $retval in
						$PARSE_OPTION_UNKNOWN_OPTION)
							printf-error 'unknown option "%s"' "$option"
							return 1
							;;

						*)
							(( parameter_index += retval ))
							: # so the result of arithmetic above does not considered as a result of case-clause execution
							;;
					esac
				done

				continue
				;;

			$PARAM_TYPE_DELIMITER)
				(( ++parameter_index ))
				break
				;;

			$PARAM_TYPE_BAD)
				printf-error 'bad parameter format "%s"' "$parameter"
				return 1
				;;
		esac
	done

	if [ $parameter_index == ${#parameters[@]} ]
	then
		return
	fi

	if [ $parameter_type == $PARAM_TYPE_DELIMITER ]
	then
		for (( ; parameter_index < ${#parameters[@]}; ++parameter_index ))
		do
			local path="${parameters[$parameter_index]}"
			local extension=$(path-extension "$path")
			if array-contains-item "$extension" "${HEADER_EXTENSIONS[@]}"
			then
				header_files+=("$path")
			elif array-contains-item "$extension" "${SOURCE_EXTENSIONS[@]}"
			then
				source_files+=("$path")
			else
				printf-error 'unknown file extension "%s"' "$path"
				return 1
			fi
		done
	fi
}


printf-message()
{
	"$PRINTF_VERBOSE_FUNCTION" "$@"
}


make-include-guard-id()
{
	local file_name=$1

	sed -e 's/-/_/g' -e 's/\./_/g' <<< "$file_name" | tr '[a-z]' '[A-Z]'
}


make-include-guard-header()
{
	"$MAKE_INCLUDE_GUARD_HEADER_FUNCTION" "$@"
}


make-include-guard-footer()
{
	"$MAKE_INCLUDE_GUARD_FOOTER_FUNCTION" "$@"
}


do-make-include-guard-pragma-once()
{
	printf '#pragma once'
}


do-make-include-guard-ifdef()
{
	local id=$1

	printf '#ifndef %s\n#define %s' "$id" "$id"
}


do-make-include-guard-endif()
{
	local id=$1

	printf '#endif // %s' "$id"
}


process-standard-header()
{
	"$PROCESS_STANDARD_HEADER_FUNCTION" "$@"
}


add-standard-include-line()
{
	local include_line=$4

	printf '%s' $(trim-string "$include_line")
}


prepend-with-standard-includes()
{
	"$PREPEND_WITH_STANDARD_INCLUDES_FUNCTION" "$@"
}


do-prepend-with-standard-includes()
{
	local output_header_path=$1
	local standard_include_list=("${@:2}")

	file-printf-prepend "$output_header_path" '%s\n\n' "$(IFS=$'\n'; sort <<< "${standard_include_list[*]}")"
}


file-printf-annotation()
{
	local output_path=$1
	local format=$2
	local arguments=("${@:3}")

	"$FILE_PRINTF_ANNOTATION_FUNCTION" "$output_path" "$format" "${arguments[@]}"
}


file-print-header-annotation()
{
	local output_path=$1
	local file_path=$2
	local indent=$3

	"$FILE_PRINT_HEADER_ANNOTATION_FUNCTION" "$output_path" "$file_path" "$indent"
}


do-file-print-header-annotation()
{
	local output_path=$1
	local file_path=$2
	local indent=$3

	local header_string=$(make-header-string "$file_path" "$indent")
	file-printf "$output_path" '%s\n' "$header_string"
}


file-print-footer-annotation()
{
	local output_path=$1
	local file_path=$2
	local indent=$3

	"$FILE_PRINT_FOOTER_ANNOTATION_FUNCTION" "$output_path" "$file_path" "$indent"
}


do-file-print-footer-annotation()
{
	local output_path=$1
	local file_path=$2
	local indent=$3

	local footer_string=$(make-footer-string "$file_path" "$indent")
	file-printf "$output_path" '%s\n\n' "$footer_string"
}


indent-shift()
{
	local indent=$1

	"$INDENT_SHIFT_FUNCTION" "$indent"
}


do-indent-shift()
{
	local indent=$1

	printf '%s	' "$indent"
}


split-string()
{
	local string=$1
	local delimiter=$2

	IFS="$delimiter"
	local array=($string)
	unset IFS

	printf '%s\n' "${array[@]}"
}


array-contains-item()
{
	local item=$1
	local array=("${@:2}")

	local current
	for current in ${array[@]}
	do
		if [ "$current" == "$item" ]
		then
			return 0
		fi
	done

	return 1
}


find-by-extensions()
{
	local path=$1
	local extensions=("${@:2}")

	local arguments=" -name '*.${extensions[0]}'"
	local extension
	for extension in "${extensions[@]:1}"
	do
		arguments+=" -or -name '*.$extension'"
	done

	eval 'find '"$path"' -type f '"$arguments"
}


is-include-line()
{
	local line=$1

	[[ $line =~ ^[\ \	]*\#[\ \t]*include[\ \t]*\".*\".* ]]
}


is-standard-include-line()
{
	local line=$1

	[[ $line =~ ^[\ \	]*\#[\ \t]*include[\ \t]*\<.*\>.* ]]
}


is-pragma-once-line()
{
	local line=$1

	[[ $line =~ ^[\ \	]*\#[\ \t]*pragma[\ \t]*once.* ]]
}


is-empty-line()
{
	local line=$1

	[[ $line =~ ^[\ \	]*$ ]]
}


path-extension()
{
	local path=$1

	printf '%s\n' "${path##*.}"
}


detect-extension()
{
	local default_extension=$1
	local first_path=$2
	local paths=("${@:3}")

	local first_extension=$(path-extension "$first_path")
	local path
	for path in "${paths[@]}"
	do
		local extension=$(path-extension "$path")
		if [ "$first_extension" != "$extension" ]
		then
			printf '%s\n' "$default_extension"
			return 0
		fi
	done

	printf '%s\n' "$first_extension"
	return 0
}


expand-dir-path()
{
	local path=$1

	local absolute_dir_path=$(cd $path; pwd)

	printf '%s\n' "$absolute_dir_path"
}


expand-path()
{
	local path=$1

	local dir=$(dirname "$path")
	local base_name=$(basename "$path")
	local absolute_dir_path=$(expand-dir-path "$dir")

	printf '%s\n' "$absolute_dir_path/$base_name"
}


relative-path()
{
	local path=$1
	local base_path=$2

	local relative_path="${path#$base_path}"
	if [ "$relative_path" != "$path" ]
	then
		relative_path=".$relative_path"
	fi

	printf '%s\n' "$relative_path"
}


path-from-include-line()
{
	local line=$1

	local file_path=${line#*\"}
	local file_path=${file_path%\"*}

	printf '%s\n' "$file_path"
}


path-from-standard-include-line()
{
	local line=$1

	local file_path=${line#*\<}
	local file_path=${file_path%\>*}

	printf '%s\n' "$file_path"
}


absolute-path-from-include-line()
{
	local line=$1

	local file_path=${line#*\"}
	local file_path=${file_path%\"*}
	expand-path "$file_path"
}


file-printf()
{
	local file_path=$1
	local format=$2
	local arguments=("${@:3}")

	printf "$format" "${arguments[@]}" >> "$file_path"
}


file-printf-prepend()
{
	local file_path=$1
	local format=$2
	local arguments=("${@:3}")

	printf '%s' "$(printf "$format" "${arguments[@]}"; printf '%s' $(cat "$file_path"))" > "$file_path"
}


make-header-string()
{
	local line=$1
	local indent=$2

	printf "$HEADER_STRING_TEMPLATE" "$indent" "$indent" "$line" "$indent"
}


make-footer-string()
{
	local line=$1
	local indent=$2

	printf "$FOOTER_STRING_TEMPLATE" "$indent" "$indent" "$line" "$indent"
}


resolve-include-path()
{
	local include_path=$1
	local absolute_file_path=$2

	local dir=$(dirname "$absolute_file_path")
	local non_expanded_path="$dir/$include_path"

	if [ -f "$non_expanded_path" ]
	then
		expand-path "$non_expanded_path"
		return
	fi

	local absolute_include_path="$HEADER_SEARCH_PATH/$include_path"
	if [ ! -f "$absolute_include_path" ]
	then
		return 1
	fi

	printf '%s\n' "$absolute_include_path"
}


get-tag()
{
	local line=$1

	if ! [[ "$line" =~ ^.*\/\/[\ \	]*\[[\ \	]*amalgamate:[\ \	]*[-a-z][-a-z]*[\ \	]*\][\ \	]*$ ]]
	then
		return
	fi

	sed -e 's/^.*\/\/[\ \	]*\[[\ \	]*amalgamate:[\ \	]*//' -e 's/[\ \	]*\].*$//' <<< "$line"
}


get-clean-line()
{
	local line=$1

	sed -e 's/[\ \t]*\/\/[\ \t]*\[amalgamate:[\ \t]*[-a-z][-a-z]*[\ \t]*\][\ \t]*$//' <<< "$line"
}


declare -a included_files
declare -a globally_included_standard_files
declare -a locally_included_standard_files

expand-file()
{
	IFS=''
	local absolute_file_path=$1
	local output_path=$2
	local indent=$3

	local relative_file_path=$(relative-path "$absolute_file_path" $(pwd))
	printf-message '%s\n' "$relative_file_path"

	file-print-header-annotation "$output_path" "$relative_file_path" "$indent"

	local line
	while read -r line || [ -n "$line" ]
	do
		local tag=$(get-tag "$line")
		if [ "$tag" == 'leave' ]
		then
			local clean_line=$(get-clean-line "$line")
			file-printf "$output_path" '%s%s\n' "$indent" "$clean_line"
			continue
		elif [ "$tag" == 'remove' ]
		then
			continue
		elif [ "$tag" == 'uncomment' ]
		then
			local clean_line=$(get-clean-line "$line")
			line="${clean_line#*//}"
		fi

		if is-include-line "$line"
		then
			local include_path=$(path-from-include-line "$line")
			local absolute_include_path=$(resolve-include-path "$include_path" "$absolute_file_path")
			if [ -z "$absolute_include_path" ]
			then
				printf-error 'cannot find include file "%s" required in "%s"' "$include_path" "$absolute_file_path"
				exit 1
			fi

			if array-contains-item "$absolute_include_path" "${included_files[@]}"
			then
				file-printf-annotation "$output_path" '%s// %s    // amalgamate: file already expanded\n' "$indent" "$line"
				continue
			fi

			included_files+=("$absolute_include_path")
			expand-file "$absolute_include_path" "$output_path" "$(indent-shift "$indent")"

			continue
		fi

		if is-pragma-once-line "$line"
		then
			file-printf-annotation "$output_path" '%s// %s' "$indent" "$line"
			continue
		fi

		if is-empty-line "$line"
		then
			file-printf "$output_path" "%s\n" "$line"
			continue
		fi

		if is-standard-include-line "$line"
		then
			local standard_include_path=$(path-from-standard-include-line "$line")
			if array-contains-item "$standard_include_path" "${globally_included_standard_files[@]}"
			then
				file-printf-annotation "$output_path" '%s// %s    // amalgamate: file already included\n' "$indent" "$line"
				continue
			fi

			globally_included_standard_files+=("$standard_include_path")
			locally_included_standard_files+=($(process-standard-header "$output_path" '%s%s\n' "$indent" "$line"))
			continue
		fi

		file-printf "$output_path" '%s%s\n' "$indent" "$line"
	done < "$absolute_file_path"

	file-print-footer-annotation "$output_path" "$relative_file_path" "$indent"
}


merge-headers()
{
	local output_header_path=$1
	local paths=("${@:2}")

	locally_included_standard_files=()

	local path
	for path in "${paths[@]}"
	do
		local file_absolute_path=$(expand-path "$path")

		if array-contains-item "$file_absolute_path" "${included_files[@]}"
		then
			continue
		fi

		included_files+=("$file_absolute_path")
		expand-file "$file_absolute_path" "$output_header_path" ''
	done

	prepend-with-standard-includes "$output_header_path" "${locally_included_standard_files[@]}"
}


merge-sources()
{
	local output_source_path=$1
	local paths=("${@:2}")

	locally_included_standard_files=()

	local path
	for path in "${paths[@]}"
	do
		local file_absolute_path=$(expand-path "$path")

		expand-file "$file_absolute_path" "$output_source_path" ''
	done

	prepend-with-standard-includes "$output_source_path" "${locally_included_standard_files[@]}"
}


reduce-empty-lines()
{
	local file_path=$1
	local empty_line_limit=$2

	local temp="$file_path.tmp"
	mktemp "$temp" &> /dev/null

	local empty_line_count=0
	local line
	while read -r line || [ -n "$line" ]
	do
		if [ -z "$line" ]
		then
			((++empty_line_count))
			if (( empty_line_count > empty_line_limit ))
			then
				continue
			fi
		else
			empty_line_count=0
		fi

		printf '%s\n' "$line" >> "$temp"
	done < "$file_path"

	mv -f "$temp" "$file_path"
}


trim-string()
{
	local string=$1

	sed -e 's/^[	 ][	 ]*//g' -e 's/[	 ][	 ]*$//g' <<< "$string"
}


trim-trailing-whitespace()
{
	local file_path=$1

	sed -i.bak 's/[	 ][	 ]*$//g' "$file_path"
	if [ $? != 0 ]
	then
		exit 1
	fi

	rm "$file_path.bak"
}


#
# Main

parse-parameters "$@" && validate-options
if [ $? != 0 ]
then
	exit 1
fi


OUTPUT_DIR_PATH="$OUTPUT_DIR_PATH/$NAME-$OUTPUT_DIRECTORY_SUFFIX"
rm -rf "$OUTPUT_DIR_PATH" &> /dev/null
if [ $? != 0 ]
then
	printf-error 'cannot remove output directory "%s"' "$OUTPUT_DIR_PATH"
	exit 1
fi

mkdir -p "$OUTPUT_DIR_PATH"
if [ $? != 0 ]
then
	printf-error 'cannot write path "%s"' "$OUTPUT_DIR_PATH"
	exit 1
fi


if [ -z "$header_files" ] && [ -z "$source_files" ]
then
	header_files=($(find-by-extensions . "${HEADER_EXTENSIONS[@]}"))
	source_files=($(find-by-extensions . "${SOURCE_EXTENSIONS[@]}"))
fi

if [ "${#header_files[@]}" == 0 ] && [ "${#source_files[@]}" == 0 ]
then
	printf-error 'no C/C++ files found'
	exit 1
fi


if [ "$OUTPUT_HEADER_EXTENSION" == -1 ]
then
	OUTPUT_HEADER_EXTENSION=$(detect-extension "$DEFAULT_OUTPUT_HEADER_EXTENSION" "${header_files[@]}")
fi

if [ "$OUTPUT_SOURCE_EXTENSION" == -1 ]
then
	OUTPUT_SOURCE_EXTENSION=$(detect-extension "$DEFAULT_OUTPUT_SOURCE_EXTENSION" "${source_files[@]}")
fi

header_file_name="$NAME.$OUTPUT_HEADER_EXTENSION"
source_file_name="$NAME.$OUTPUT_SOURCE_EXTENSION"

output_header_path="$OUTPUT_DIR_PATH/$header_file_name"
output_source_path="$OUTPUT_DIR_PATH/$source_file_name"


# Printing options

printf-message 'Header search path: %s\n' "$HEADER_SEARCH_PATH"
printf-message 'Input header extensions: %s\n' "${HEADER_EXTENSIONS[*]}"
printf-message 'Input source extensions: %s\n' "${SOURCE_EXTENSIONS[*]}"
printf-message 'Base name: %s\n' "$NAME"
if $HEADER_ONLY
then
	printf-message 'Header only\n'
	printf-message 'Output header extension: %s\n' "$OUTPUT_HEADER_EXTENSION"
elif $SOURCE_ONLY
then
	printf-message 'Source only'
	printf-message 'Output source extension: %s\n' "$OUTPUT_SOURCE_EXTENSION"
else
	printf-message 'Output header extension: %s\n' "$OUTPUT_HEADER_EXTENSION"
	printf-message 'Output source extension: %s\n' "$OUTPUT_SOURCE_EXTENSION"
fi

printf-message 'Reduce consecutive empty lines to: %s\n' "$EMPTY_LINE_COUNT_LIMIT"

if $TRIM_TRAILING_WHITESPACE
then
	printf-message 'Trim trailing whitespace: yes\n'
else
	printf-message 'Trim trailing whitespace: no\n'
fi

if [ "$FILE_PRINTF_ANNOTATION_FUNCTION" != ':' ]
then
	printf-message 'Insert annotations: yes\n'
else
	printf-message 'Insert annotations: no\n'
fi

printf-message '\nSaving amalgamated files to:\n%s/\n' "$OUTPUT_DIR_PATH"
if $HEADER_ONLY
then
	output_source_path="$output_header_path"
	printf-message '└── %s\n\n' "$header_file_name"
elif $SOURCE_ONLY
then
	output_header_path="$output_source_path"
	printf-message '└── %s\n\n' "$source_file_name"
else
	printf-message '├── %s\n└── %s\n\n' "$header_file_name" "$source_file_name"
fi


rm -f "$output_header_path" "$output_source_path"

included_files+=("$output_header_path")

if [ ${#header_files[@]} != 0 ]
then
	printf-message 'Amalgamating headers...\n'

	merge-headers "$output_header_path" "${header_files[@]}"

	if ! $SOURCE_ONLY
	then
		INCLUDE_GUARD_ID=$(make-include-guard-id "$header_file_name")

		include_guard_header=$(make-include-guard-header "$INCLUDE_GUARD_ID")
		file-printf-prepend "$output_header_path" '%s\n\n' "${include_guard_header}"

		include_guard_footer=$(make-include-guard-footer "$INCLUDE_GUARD_ID")
		file-printf "$output_header_path" '\n\n%s' "$include_guard_footer"
	fi

	printf-message '\n'
fi

if [ ${#source_files[@]} != 0 ]
then
	printf-message 'Amalgamating sources...\n'
	if [ ${#header_files[@]} != 0 ] && ! $SOURCE_ONLY
	then
		file-printf "$output_source_path" '#include "%s"\n' "$(basename $header_file_name)"
	fi

	merge-sources "$output_source_path" "${source_files[@]}"

	printf-message '\n'
fi

if $TRIM_TRAILING_WHITESPACE
then
	printf-message 'Trimming trailing whitespace...\n'

	if [ -r "$output_header_path" ]
	then
		trim-trailing-whitespace "$output_header_path"
	fi

	if [ -r "$output_source_path" ]
	then
		trim-trailing-whitespace "$output_source_path"
	fi
fi

if [ -n "$EMPTY_LINE_COUNT_LIMIT" ]
then
	printf-message 'Reducing empty lines...\n'

	if [ -r "$output_header_path" ]
	then
		reduce-empty-lines "$output_header_path" $EMPTY_LINE_COUNT_LIMIT
	fi

	if [ -r "$output_source_path" ]
	then
		reduce-empty-lines "$output_source_path" $EMPTY_LINE_COUNT_LIMIT
	fi
fi

printf-message '\nDone.\n'

