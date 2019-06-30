import tlib
class LogCapture(object):
	def __init__(self):
		self.buf = ''

	def write(self, data):
		lines = data.splitlines(True)
		for singleLine in lines:
			if singleLine[-1] == '\n':
				msg = singleLine[0:-1]
				if self.buf:
					msg = self.buf + msg
					self.buf = ""
				tlib.log(msg)
			else:
				self.buf += singleLine

def init():
	import sys
	sys.stderr = sys.stdout = LogCapture()