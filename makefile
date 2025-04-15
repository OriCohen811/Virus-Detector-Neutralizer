VIRUS = AntiVirus

Objects = AntiVirus
Ofiles = AntiVirus.o

all: clean $(Objects) clean.o

$(VIRUS): $(VIRUS).o
	gcc -g -Wall -o $(VIRUS) $(VIRUS).o

$(VIRUS).o: $(VIRUS).c
	gcc -g -c -Wall -o $(VIRUS).o $(VIRUS).c

clean.o: 
	rm -f $(Ofiles)

clean:
	rm -f $(Objects)