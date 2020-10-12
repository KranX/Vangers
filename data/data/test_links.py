# -*- coding: utf-8 -*-
import sys, os


file_list = [
	"B-Zone",
	"Incubator",
	"Lampasso",
	"Ogorod",
	"Podish",
	"Spobs",
	"Threall",
	"VigBoo",
	"ZeePa",
	"Inventory"
]

dir_path = sys.argv[1]

def is_ascii(s):
    return all(ord(c) < 128 for c in s)

def have_3dot(s):
	return all(ord(c) != 133 for c in s)

def parse_text(text):
	START_FIND = 0
	TITLE = 1
	TEXT = 2
	LINK = 3
	COMMAND = 4

	text_dict = dict()
	buf_string = ""
	current_title = ""
	state_fsm = START_FIND
	end_symbol = None
	for c in text:
		if state_fsm == START_FIND:
			if c == "[":
				buf_string = ""
				state_fsm = TITLE
				continue
			elif c == "\"":
				buf_string = ""
				state_fsm = TEXT
				continue
			elif c == "@":
				buf_string = ""
				state_fsm = COMMAND
				continue
			elif c == "{":
				buf_string = ""
				state_fsm = LINK
				continue

		elif state_fsm == TITLE:
			if c == "]":
				current_title = buf_string
				text_dict[current_title] = {
					"texts_ru": [],
					"texts_en": [],
					"links_ru": [],
					"links_en": [],
					"commands": []
				}
				state_fsm = START_FIND
				#print "TITLE:", current_title
			else:
				buf_string += c
		elif state_fsm == TEXT:
			if c == "\"":
				if not have_3dot(buf_string):
					print "ERROR: detect 3dot symbol"
					print "TEXT:", buf_string.decode("cp1251")
				if ord(buf_string[0]) < 128:
					if not is_ascii(buf_string):
						print "ERROR: first char is ascii but text have cp1251 symbol"
						print "TEXT:", buf_string.decode("cp1251")
						print "SYMBOLS:",
						for i, c2 in enumerate(buf_string):
							if ord(c2) >= 127:
								print str(i)+":"+c2.decode("cp1251"), 
						print ""
					text_dict[current_title]["texts_en"].append(buf_string)
				else:
					text_dict[current_title]["texts_ru"].append(buf_string)

				state_fsm = START_FIND
				
			else:
				buf_string += c
		elif state_fsm == COMMAND:
			if c == "\n":
				text_dict[current_title]["commands"].append(buf_string)
				state_fsm = START_FIND
			else:
				buf_string += c
		elif state_fsm == LINK:
			if c == "}":
				if ord(buf_string[0]) < 128:
					if not is_ascii(buf_string):
						print "ERROR LINK: first char is ascii but text have cp1251 symbol"
						print "LINK:", buf_string.decode("cp1251")
					text_dict[current_title]["links_en"].append(buf_string)
					if not text_dict[current_title]["texts_en"]:
						print "ERROR LINK: link write before text in", current_title
				else:
					text_dict[current_title]["links_ru"].append(buf_string)
					if not text_dict[current_title]["texts_ru"]:
						print "ERROR LINK: link write before text in", current_title
				state_fsm = START_FIND
			else:
				buf_string += c

	return text_dict

def parse_query(text):
	START_FIND = 0
	TITLE = 1
	TEXT = 2
	LINK = 3
	
	query_dict_eng = {"null":{"links":[], "texts":[]}}
	query_dict_rus = {"null":{"links":[], "texts":[]}}
	pages_rus = []
	pages_eng = []
	links_rus = []
	links_eng = []
	cur_page_eng = query_dict_eng["null"]
	cur_page_rus = query_dict_rus["null"]
	buf_string = ""
	state_fsm = START_FIND
	end_symbol = None
	for c in text:
		if state_fsm == START_FIND:
			if c == "[":
				buf_string = ""
				state_fsm = TITLE
				continue
			elif c == "{":
				buf_string = ""
				state_fsm = LINK
				continue
			elif c == "\"":
				buf_string = ""
				state_fsm = TEXT
				continue
		elif state_fsm == TITLE:
			if c == "]":
				if ord(buf_string[0]) < 128:
					pages_eng.append(buf_string)
					cur_page_eng = {"links":[], "texts":[]}
					query_dict_eng[buf_string] = cur_page_eng
				else:
					pages_rus.append(buf_string.decode("cp1251"))
					cur_page_rus = {"links":[], "texts":[]}
					query_dict_rus[buf_string] = cur_page_rus
				state_fsm = START_FIND
				#print "TITLE:", current_title
			else:
				buf_string += c
		elif state_fsm == LINK:
			if c == "}":
				if ord(buf_string[0]) < 128:
					if not is_ascii(buf_string):
						print "QUERY ERROR LINK: first char is ascii but text have cp1251 symbol"
						print "QUERY LINK:", buf_string.decode("cp1251")
					links_eng.append(buf_string)
					cur_page_eng["links"].append(buf_string)
				else:
					links_rus.append(buf_string)
					cur_page_rus["links"].append(buf_string)
				state_fsm = START_FIND
			else:
				buf_string += c
		elif state_fsm == TEXT:
			if c == "\"":
				if not have_3dot(buf_string):
					print "QUERY ERROR: detect 3dot symbol"
					print "QUERY TEXT:", buf_string.decode("cp1251")
				if ord(buf_string[0]) < 128:
					if not is_ascii(buf_string):
						print "QUERY ERROR: first char is ascii but text have cp1251 symbol"
						print "QUERY TEXT:", buf_string.decode("cp1251")
						print "QUERY SYMBOLS:",
						for i, c2 in enumerate(buf_string):
							if ord(c2) >= 127:
								print str(i)+":"+c2.decode("cp1251"), 
						print ""
					cur_page_eng["texts"].append(buf_string)
				else:
					cur_page_rus["texts"].append(buf_string)

				state_fsm = START_FIND
				
			else:
				buf_string += c
		
	
	return pages_eng, pages_rus, links_eng, links_rus, query_dict_eng, query_dict_rus

def test_file(file_name):
	text_file = open(file_name+".text")
	query_file = open(file_name+".query")
	text = text_file.read()
	query = query_file.read()

	text_dict = parse_text(text)
	eng_pages, rus_pages, eng_query_links, rus_query_links, query_dict_eng, query_dict_rus = parse_query(query)
	

	for link in rus_query_links:
		if link.decode("cp1251") not in rus_pages:
			print "Not found page for query link:", link.decode("cp1251")
		else:
			#print "Found page for link:", link.decode("cp1251")
			pass

	for link in eng_query_links:
		if link not in eng_pages:
			print "Not found page for query link:", link
		else:
			#print "Found page for link:", link.decode("cp1251")
			pass

	for title in text_dict:
		print "Title:", title
		for link in text_dict[title]["links_ru"]:
			if link.decode("cp1251") not in rus_pages:
				print "Not found page for link:", link.decode("cp1251")
			else:
				#print "Found page for link:", link.decode("cp1251")
				pass

		for link in text_dict[title]["links_en"]:
			if link not in eng_pages:
				print "Not found page for link:", link
			else:
				#print "Found page for link:", link.decode("cp1251")
				pass

		
		#print "Detect link in text"
		for string in text_dict[title]["texts_ru"]:
			if string in rus_pages:
				print "Detect link in text", string.decode("cp1251")
			if string[0] == " ":
				print "String start at space", string.decode("cp1251")
			for text in [
				u"он сдуревших циппо",
				u"cмог",
				u"cредь",
				u"Эскэйв",
				u"невидано",
				u"втречных",
				u"предаешся",
				u"начто",
				u"заботюсь",
				u"захотелось в Подиш?",
				u"свяэано",
				u"?.",
				u"готовся",
				u"читать не могут",
				u"поже ",
				u"не лучшие времена...",
				u"славюсь",
				u"бесприкословно",
				u"чужых",
				u"целостноть",
				u"—"
			]:
				if text.encode("cp1251") in string:
					print "Detect wrong word", text, " in ", string.decode("cp1251")

		for string in text_dict[title]["texts_en"]:
			if string in eng_pages:
				print "Detect link in text", string
			if string[0] == " ":
				print "String start at space", string

	for query in query_dict_eng:
		for text in query_dict_eng[query]["texts"]:
			if len(text) > 340:
				print "QUERY In", query.decode("cp1251"), " text have more 340 chars -", len(text), ":"
				print text.decode("cp1251")
		
	for query in query_dict_rus:
		for text in query_dict_rus[query]["texts"]:
			if len(text) > 340:
				print "QUERY In", query.decode("cp1251"), " text have more 340 chars -", len(text), ":"
				print text.decode("cp1251")
			
		#print "Eng text:"
		#for string in text_dict[title]["texts_en"]:
		#	print string

		#if text_dict[title]["links_ru"]:
		#	print "Rus links:"
		#	for string in text_dict[title]["links_ru"]:
		#		print "{"+string.decode("cp1251")+"}", 
		#	print ""

		#if text_dict[title]["links_en"]:
		#	print "Eng links:"
		#	for string in text_dict[title]["links_en"]:
		#		print "{"+string.decode("cp1251")+"}",
		#	print ""


for file_name in file_list:
	print os.path.join(dir_path, file_name)
	test_file(file_name)
