// Template for parsing an ELF file to print its symbol table
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

// The below macros help to prevent errors when doing pointer
// arithmetic. Adding an offset to a pointer is the most common
// operation here while the other macros may not be needed.

#define PTR_PLUS_BYTES(ptr,off) ((void *) (((size_t) (ptr)) + ((size_t) (off))))
// macro to add a byte offset to a pointer, arguments are a pointer
// and a # of bytes (usually size_t)

#define PTR_MINUS_BYTES(ptr,off) ((void *) (((size_t) (ptr)) - ((size_t) (off))))
// macro to subtract a byte offset from a pointer, arguments are a pointer
// and a # of bytes (usually size_t)

#define PTR_MINUS_PTR(ptr,ptq) ((long) (((size_t) (ptr)) - ((size_t) (ptq))))
// macro to subtract one pointer from another

int DEBUG = 0;                  
// Controls whether to print debug messages
// Can be used in conditionals like if(DEBUG){ ... }
// and running 'showsym -d x.o' will set DEBUG=1

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("usage: %s [-d] <file>\n",argv[0]);
    return 0;
  }

  char *objfile_name = argv[1];

  // check for debug mode
  if(argc >=3){
    if(strcmp("-d",argv[1])==0){
      DEBUG = 1;
      objfile_name = argv[2];
    }
    else{
      printf("incorrect usage\n");
      return 1;
    }
  }
                        
  // Opened file descriptor and set up memory map for objfile_name
  int file = open(objfile_name, O_RDONLY);
  struct stat buf;
  fstat(file, &buf);
  void *src = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, file, 0);

  // CREATED A POINTER to the intial bytes of the file which are an ELF64_Ehdr struct
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)((char *)src);

  // Checking e_ident field's bytes 0 to for for the sequence {0x7f,'E','L','F'}.
  // Exiting the program with code 1 if the bytes do not match
  if (ehdr->e_ident[0] != '\x7f' || ehdr->e_ident[1] != 'E' || ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F'){
    printf("ERROR: Magic bytes wrong, this is not an ELF file\n"); 
    return 1;
  }


  // PROVIDED: check for a 64-bit file
  if(ehdr->e_ident[EI_CLASS] != ELFCLASS64){
    printf("Not a 64-bit file ELF file\n");
    return 1;
  }

  // PROVIDED: check for x86-64 architecture
  if(ehdr->e_machine != EM_X86_64){
    printf("Not an x86-64 file\n");
    return 1;
  }

  // DETERMINED THE OFFSET of the Section Header Array (e_shoff), the
  // number of sections (e_shnum), and the index of the Section Header
  // String table (e_shstrndx). These fields are from the ELF File
  // Header.
  int offset_of_header_array = ehdr->e_shoff;            //THE OFFSET of the Section Header Array (e_shoff),
  int number_of_section = ehdr->e_shnum;                 // index of the Section Header String table (e_shstrndx
  int index_of_header_string_table= ehdr->e_shstrndx;   // String table (e_shstrndx)
  
  
  // Did set et up a pointer to the array of section headers and Used the section
  // header string table index to find its byte position in the file
  // and did set up a pointer to it.
  
  Elf64_Shdr *the_array_of_section_headers = (Elf64_Shdr *)((char *)src + offset_of_header_array);        //pointer to the array of section headers 
  Elf64_Shdr *the_section_string_table_header = the_array_of_section_headers + index_of_header_string_table; // indexed to find its byte position in the file
  char *the_section_string_table = (char *)((char *)src + the_section_string_table_header->sh_offset);     // did set up a pointer to the position of string table index
  
  int symbol_table_offset=0;                                      //the offset of symbol table initialized to 0
  int string_table_offset=0;                                      //the offset of string table initialized to 0
  int total_size, each_entry_size, number_etries;               // variable for bytes total size, bytes per entry and entries
  Elf64_Shdr *the_section_head = (Elf64_Shdr *)the_array_of_section_headers;
  for (int i = 0; i < number_of_section; i++){                  //going over the section header string table
    char *name = the_section_string_table + the_section_head->sh_name;    // initializing the name for further check 
    if (strcmp(name, ".symtab") == 0){                           //checking for .symtab
      symbol_table_offset = the_section_head->sh_offset;
      total_size = the_section_head->sh_size;
      each_entry_size = the_section_head->sh_entsize;
      number_etries = the_section_head->sh_size / the_section_head->sh_entsize;
    }
    if (strcmp(name, ".strtab") == 0){            //checking for .strtab
      string_table_offset = the_section_head->sh_offset;
    }
    the_section_head++;
  }
  if (symbol_table_offset==0){     //ERROR checking
    printf("ERROR: Couldn't find symbol table\n");
    return 1;
  }

  if (string_table_offset==0){    //ERROR checking
    printf("Couldn't find string table\n");
    return 1;
  }

  // PRINTing byte information about where the symbol table was found and
  // its sizes. The number of entries in the symbol table can be
  // determined by dividing its total size in bytes by the size of
  // each entry.
  printf("Symbol Table\n");
  printf("- %lu bytes offset from start of file\n", (size_t)symbol_table_offset);
  printf("- %lu bytes total size\n", (size_t)total_size);
  printf("- %lu bytes per entry\n", (size_t)each_entry_size);
  printf("- %lu entries\n", (size_t)number_etries);

  // Set up pointers to the Symbol Table and associated String Table
  // using offsets found earlier.

  Elf64_Sym *symbol_table = (Elf64_Sym *)((char *)src + symbol_table_offset);
  char *string_table = (char *)((char *)src + string_table_offset);

  // Print column IDs for info on each symbol
  printf("[%3s]  %8s %4s %s\n",
         "idx", "TYPE", "SIZE", "NAME");

  // Iterating over the symbol table entries
  for (int i = 0; i < number_etries; i++){
    // Determining the size of symbol and name. Use <NONE> name has zero
    // length.
    char *name_ptr = string_table + symbol_table->st_name;
    char name[128] = {0};
    if (strlen(name_ptr)!=0){
      strcpy(name, name_ptr);
    }
    else{
      strcpy(name, "<NONE>");
    }

    // Determine type of symbol. See assignment specification for
    // fields, macros, and definitions related to this.
    char type[64] = {0};
    if (ELF64_ST_TYPE(symbol_table->st_info) == 0){
      strcpy(type, "NOTYPE");
    }
    else if (ELF64_ST_TYPE(symbol_table->st_info) == 1){
      strcpy(type, "OBJECT");
    }
    else if (ELF64_ST_TYPE(symbol_table->st_info) == 2){
      strcpy(type, "FUNC");
    }
    else if (ELF64_ST_TYPE(symbol_table->st_info) == 3){
      strcpy(type, "SECTION");
    }
    else if (ELF64_ST_TYPE(symbol_table->st_info) == 4){
      strcpy(type, "FILE");
    }
    int size = symbol_table->st_size;

    // Print symbol information
    printf("[%3d]: %8s %4lu %s\n", i, type, (size_t)size, name);
    symbol_table++;
  }

  // Unmap file from memory and close associated file descriptor
  close(file);
  munmap(src, buf.st_size);
  return 0;
}
