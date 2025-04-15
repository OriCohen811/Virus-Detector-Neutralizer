#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct link link;
typedef struct virus virus;

FILE* input;
FILE* output;
link* virus_list = NULL;

void LoadSign();
void printSign();
void detectVir();
void fixFile();
void quit();

void detect_virus(char *buffer, unsigned int size, link *virus_list);
void neutralize_virus(char *fileName, int signatureOffset);

struct link {
    link *nextVirus;
    virus *vir;
};

typedef struct virus {
    unsigned short SigSize;
    unsigned char* VirusName;
    unsigned char* Sig;
} virus;

struct fun_desc{
    char* name;
    void (*fun)();
};

void virusDestructor(virus* v){
    if(v!=NULL && v->VirusName!=NULL){
        free(v->VirusName);
    }
    if(v!=NULL && v->Sig!=NULL){
        free(v->Sig);
    }
    if(v!=NULL){
        free(v);
    }
}

void errorClosing(unsigned char* buffer, virus* v, char* msg){
    if(buffer!= NULL){
        free(buffer);
    }
    if(v!=NULL){
        virusDestructor(v);
    }
    if(msg!=NULL && *msg!='\0'){
        printf("%s", msg);
    }
}

link *list_append(link *virus_list, virus *data);
void list_print(link *virus_list, FILE *file);
void list_free(link *virus_list);

/*
this function receives a file pointer and returns a virus* that represents the next virus in the file. 
To read from a file, use fread(). See man fread(3) for assistance.
*/
virus* readVirus(FILE* file){
    if (file == NULL) {
        printf("Invalid file pointer.\n");
        return NULL;
    }
    size_t n;
    unsigned char* buffer = NULL;
    virus* newVirus = (virus*) calloc(1, sizeof(virus));
    if (newVirus == NULL) {
        errorClosing(buffer, newVirus,"Memory allocation failed for virus struct.\n");
        return NULL;
    }

    //SigSize checking
    buffer = (unsigned char*)malloc(2*sizeof(unsigned char)); 
    if(buffer==NULL){
        errorClosing(buffer, newVirus, "Memory allocation failed\n");
        return NULL;
    }
    n = fread(buffer, sizeof(unsigned char), 2, file);
    if(n<2){
        errorClosing(buffer, newVirus, "Failed to read virus size.\n");
        return NULL;
    }
    newVirus->SigSize = buffer[0] | (buffer[1] << 8);
    free(buffer);
    buffer = NULL;
    //end of checking

    //VirusName checking
    newVirus->VirusName = (unsigned char*)malloc(16*sizeof(unsigned char));
    (newVirus->VirusName)[15] = '\0';
    n = fread(newVirus->VirusName, sizeof(unsigned char), 16, file);
    if(n<16){
        errorClosing(buffer, newVirus, "Too few arguments in the file for virus name\n");
        return NULL;
    }
    //end of checking
    
    //Sig checking
    newVirus->Sig = (unsigned char *)malloc(newVirus->SigSize*sizeof(unsigned char));
    if (newVirus->Sig == NULL) {
        errorClosing(buffer, newVirus, "Memory allocation failed\n");
        return NULL;
    }
    n = fread(newVirus->Sig, sizeof(unsigned char), newVirus->SigSize, file);
    if (n < newVirus->SigSize) {
        errorClosing(buffer, newVirus, "Failed to read complete signature\n");
        return NULL;
    }
    //end of checking
    return newVirus;
}

/*
this function receives a virus and a pointer to an output file. 
The function prints the virus to the given output. 
It prints the virus name (in ASCII), the virus signature length (in decimal), and the virus signature (in hexadecimal representation).
*/
void printVirus(virus* virus, FILE* output) {
    if (virus == NULL || output == NULL) return;

    fprintf(output, "Virus name: %s\n", virus->VirusName);
    fprintf(output, "Virus size: %hu\n", virus->SigSize);
    fprintf(output, "Signature:\n");

    for (int i = 0; i < virus->SigSize; i++) {
        fprintf(output, "%02X ", virus->Sig[i]);
    }

    fprintf(output, "\n\n");
}

void signaturesParser(char *filename) {
    FILE *inFile = fopen(filename, "rb");
    free(filename);
    FILE *outFile = stdout;
    virus *virus = NULL;
    if (inFile == NULL) {
        printf("Failed to open file.\n");
        return;
    }
    //megic word checking
    char magic[4];
    int n = fread(magic, sizeof(unsigned char), 4, inFile);
    if (n < 4 || memcmp(magic, "VIRL", 4) * memcmp(magic, "VIRB", 4) != 0) {
        printf("Invalid magic number.\n");
        return;
    }
    //end of checking

    while ((virus = readVirus(inFile)) != NULL) {
        printVirus(virus, outFile);
        virus_list = list_append(virus_list, virus);
        virus = NULL;
    }
}

/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
void list_print(link *virus_list, FILE *file){
    if(virus_list != NULL){
        printVirus(virus_list->vir, file);
        list_print(virus_list->nextVirus, file);
    }
}

/* Add a new link with the given data to the list (at the end CAN ALSO AT BEGINNING), and return a pointer to the list (i.e., the first link in the list). If the list is null - create a new entry and return a pointer to the entry. */
link *list_append(link *virus_list, virus *data){
    link* newLink = (link*)malloc(sizeof(link));
    newLink->vir = data;
    newLink->nextVirus = virus_list;
    return newLink;
}

/* Free the memory allocated by the list. */
void list_free(link *virus_list){
    if(virus_list==NULL){
        return;
    }
    link *next = virus_list->nextVirus;
    virusDestructor(virus_list->vir);
    free(virus_list);
    list_free(next);
}

void LoadSign(){
    char* fileName = (char*)malloc(120*sizeof(char));
    fprintf(output, "Enter signatures file: ");
    if(fgets(fileName, 120,input)==NULL){
        return;
    }else{
        char* p = fileName;
        while(*p!='\0' && *p!='\n'){
            p = p + 1;
        }
        if(*p=='\n'){
            *p = '\0';
        }
        signaturesParser(fileName);
    }
}

void printSign(){
    list_print(virus_list, output);
}

void fixFile() {
    if (virus_list == NULL) {
        printf("No signatures loaded. Load signatures first.\n");
        return;
    }

    char filename[256];
    printf("Enter file name to fix: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Error reading filename.\n");
        return;
    }

    char *newline = strchr(filename, '\n');
    if (newline) *newline = '\0';

    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Failed to open file %s\n", filename);
        return;
    }

    char buffer[10000];
    size_t size = fread(buffer, sizeof(unsigned char), sizeof(buffer), file);
    fclose(file);

    if (size == 0) {
        printf("Failed to read file or file is empty.\n");
        return;
    }

    for (unsigned int i = 0; i < size; i++) {
        link *curr = virus_list;
        while (curr != NULL) {
            virus *v = curr->vir;
            if (i + v->SigSize <= size && memcmp(buffer + i, v->Sig, v->SigSize) == 0) {
                printf("Found virus at offset %u\n", i);
                neutralize_virus(filename, i);
            }
            curr = curr->nextVirus;
        }
    }
}

void neutralize_virus(char *fileName, int signatureOffset) {
    FILE *file = fopen(fileName, "r+b");
    if (file == NULL) {
        printf("Error: Unable to open file for patching: %s\n", fileName);
        return;
    }

    if (fseek(file, signatureOffset, SEEK_SET) != 0) {
        printf("Error: Failed to seek to offset %d in file.\n", signatureOffset);
        fclose(file);
        return;
    }

    unsigned char retOpcode = 0xC3;
    if (fwrite(&retOpcode, sizeof(unsigned char), 1, file) != 1) {
        printf("Error: Failed to write RET instruction at offset %d.\n", signatureOffset);
    } else {
        printf("Neutralized virus at offset %d.\n", signatureOffset);
    }

    fclose(file);
}

void quit(){
    fprintf(output, "Good Bye!\n");
}

void map(void (*f)()){
    f();
}

void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    for (unsigned int i = 0; i < size; i++) {
        link *curr = virus_list;
        while (curr != NULL) {
            virus *v = curr->vir;
            if (i + v->SigSize <= size && memcmp(buffer + i, v->Sig, v->SigSize) == 0) {
                printf("Starting byte location: %u\n", i);
                printf("Virus name: %s\n", v->VirusName);
                printf("Virus size: %hu\n\n", v->SigSize);
            }
            curr = curr->nextVirus;
        }
    }
}

void detectVir() {
    char filename[256];
    printf("Enter file name to scan: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Error reading filename.\n");
        return;
    }

    char *newline = strchr(filename, '\n');
    if (newline) *newline = '\0';

    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Failed to open file %s\n", filename);
        return;
    }

    char buffer[10000];
    size_t size = fread(buffer, sizeof(unsigned char), sizeof(buffer), file);
    fclose(file);

    if (size == 0) {
        printf("Failed to read file or file is empty.\n");
        return;
    }

    detect_virus(buffer, size, virus_list);
}

int main(int argc, char **argv) {
    struct fun_desc menu[] = {{"Load signatures", LoadSign}, {"Print signatures", printSign}, {"Detect viruses", detectVir}, {"Fix file", fixFile}, {"Quit", quit}, {NULL, NULL}};
    int menuSize = sizeof(menu) / sizeof(struct fun_desc) - 1;
    int choice;

    input = stdin;
    output = stdout;

     while(1){
        char buffer[120];

        printf("Select operation from the following menu :\n");

        for (int i = 0; i < menuSize; i++){
            printf("%d)\t%s\n", i+1, menu[i].name);
        }

        printf("Option: ");
        
        if(fgets(buffer, 120, stdin) == NULL){
            printf("\n");
            break;
        }
        printf("\n");
        choice = atoi(buffer)-1;

        if(choice>=0 && choice<=menuSize){
            printf("within bounds\n");
            map(menu[choice].fun);
            if(choice==4){
                break;
            }
            printf("DONE.\n\n");
        }else{
            printf("Not within bounds\n");
            break;
        }

    }
    list_free(virus_list);
    return 0;
}