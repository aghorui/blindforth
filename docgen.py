# Generates MD files from each of the source code files.

import re
import os

# Constants
DOC_NAMES = [ "tokenizer.cpp" ]
DEST_DIR = "doc"
SRC_DIR = "."

# Regexes
md_comment_start          = re.compile(r'^\/\*\*md(.*)$')
md_comment_end            = re.compile(r'^(.*)\*\/(.*)$')
md_comment_prefix         = re.compile(r'^\ \*\ (.*)$')
md_comment_prefix_nospace = re.compile(r'^\ \*(.*)$')

# Enum sort of thing.
class State:
	NOBLOCK_START = 0
	NOBLOCK = 1
	START = 1

def prepare_document(doc):
	state = State.NOBLOCK_START
	ret = ""
	linecount = 0

	while True:
		line = doc.readline()

		# Break if end reached.
		if line == '':
			break

		linecount += 1

		# If we are starting out with the document, detect if this first is a
		# markdown comment and if not, start a code block.
		if state == State.NOBLOCK_START:
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
					print("Error at line " + linecount + ": Repeated comment start sequence.")
					return None

				# Remove initial spacing from line if it exists.
				m  = md_comment_prefix.match(line)
				if m:
					ret += m.group(1) + '\n'
					break

				# Normal '*' prefixed line with no spacing.
				m = md_comment_prefix_nospace.match(line)
				if m:
					ret += m.group(1) + '\n'
					break

				# Commen end matching.
				m = md_comment_end.match(line)
				if m:
					ret += m.group(1) + '\n'
					ret += '```' + '\n'
					ret += (m.group(2) + '\n') if m.group(2) != '' else ''
					break

				# If no match, we assume there are no special cases. Just copy
				# -paste in the line.
				ret += line
				break

	return ret

def main():
	for name in DOC_NAMES:
		path = SRC_DIR + '/' + name
		file = None
		try:
			file = open(path)
		except FileNotFoundError e:
			print("Error: '" + path + "' not found.")
			return 1
		text = prepare_document(file)
		dest = open(DEST_DIR + "/" + FILL_THIS_IN, 'w')
		dest.write(text)
		dest.close()
		file.close()

	return 0