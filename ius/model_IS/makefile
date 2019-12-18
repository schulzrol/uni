uc_src=uc.puml
er_src=er.puml

####################

CFLAGS=-nbthread auto
#defaultly compile to pdf
to=svg

all : usecase er

usecase: $(uc_src)
	plantuml $(CFLAGS) $^ -t$(to)

er: $(er_src)
	plantuml $(CFLAGS) $^ -t$(to)

$(view) : 
	zathura $^
