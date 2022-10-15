# Generates Markdown files from each of the source code files.
# Markdown comment sections in the source files are marked by "\**md".

from typing import List, Dict
import re
import sys

# Constants
DEST_DIR = "doc"
SRC_DIR  = "."

DOC_NAMES: List[Dict] = [
	{ 'src': "tokenizer.cpp", 'dest': "tokenizer.md" }
]


# Regexes
md_comment_start: re.Pattern          = re.compile(r'^\/\*\*md(.*)$')
md_comment_end: re.Pattern            = re.compile(r'^(.*)\*\/(.*)$')
md_comment_prefix: re.Pattern         = re.compile(r'^\ \*\ (.*)$')
md_comment_prefix_nospace: re.Pattern = re.compile(r'^\ \*(.*)$')

# Enum sort of thing.
class State:
	NOBLOCK_START = 0
	NOBLOCK = 1
	START = 2

def prepare_document(src) -> str:
	state: int = State.NOBLOCK_START
	ret: str = ""
	linecount: int = 0

	while True:
		line: str = src.readline()

		# Break if end reached.
		if line == '':
			break

		linecount += 1

		# If we are starting out with the document, detect if this first is a
		# markdown comment and if not, start a code block.
		# TODO Place this out of the loop.
		if state == State.NOBLOCK_START:
			m = md_comment_start.match(line)
			if m:
				state = State.START
				ret += m.group(1) + '\n'
			else:
				ret += '```\n'
				state = State.NOBLOCK

		# Document-ongoing commentblock detection
		if state == State.NOBLOCK:
			m = md_comment_start.match(line)
			if m:
				state = State.START
				ret += '```\n'
				ret += m.group(1) + '\n'
			else:
				ret += line

		# In-comment editing.
		elif state == State.START:
			# Match all the possible combinations.
			# This acts like a switch statement.
			while True:
				m = md_comment_start.match(line)
				if m:
					#DEBUG print("start matched")
					print("Error at line " + str(linecount) + ": Repeated comment start sequence.")
					return ""

				# Commen end matching. Takes precedence over prefix match.
				m = md_comment_end.match(line)
				if m:
					#DEBUG print("end matched")
					ret += m.group(1) + '\n'
					ret += '```' + '\n'
					ret += (m.group(2) + '\n') if m.group(2) != '' else ''
					state = State.NOBLOCK
					break

				# Remove initial spacing from line if it exists.
				# Takes precedence over nospace prefix.
				m  = md_comment_prefix.match(line)
				if m:
					#DEBUG print("prefix matched")
					ret += m.group(1) + '\n'
					break

				# Normal '*' prefixed line with no spacing.
				m = md_comment_prefix_nospace.match(line)
				if m:
					#DEBUG print("prefix nospace matched")
					ret += m.group(1) + '\n'
					break

				# If no match, we assume there are no special cases. Just copy
				# -paste in the line.
				#DEBUG print("default matched")
				ret += line
				break

	if state == State.NOBLOCK:
		ret += '\n```\n'

	return ret

def main():
	# Iterate over files and generate each
	for name in DOC_NAMES:
		srcpath: str = SRC_DIR + '/' + name['src']
		destpath: str = DEST_DIR + "/" + name['dest']
		file = None
		try:
			file = open(srcpath)
		except FileNotFoundError:
			print("Error: '" + srcpath + "' not found.")
			return 1

		print("Converting '" + srcpath + "' -> '" + destpath + "'")
		text: str = prepare_document(file)
		if text == '':
			print("Exiting due to error.")
			return 2

		try:
			dest = open(destpath, 'w')
		except IOError:
			print("Error: Could not open '" + destpath + "'.")
			return 1
		dest.write(text)

		dest.close()
		file.close()

	print("Done.")
	return 0

if __name__ == '__main__':
	sys.exit(main())