#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ctype.h>

#include <stdbool.h>
#include <inttypes.h>

typedef enum Type{
	
	/**
     * Enum to represent type.
     */
	
	bool_e        ,
	int_e         , 
	int_array_e   ,
	int_jagged_e  ,
	float_e       , 
	float_array_e , 
	float_jagged_e, 
	char_e        ,
	string_e      , 
	string_array_e
} type_e;

typedef union Universal_U {
	
	/**
     * Union to store commonly used types.
     */
	
	bool    b;
	int32_t i; 
	float   f;
	char    c;
	char  * s;
} uni_u;

typedef struct Universal_S {
	
	/**
     * Structure to store commonly used types and their type.
     */
	  	
	uni_u  value;
	type_e type ;
	
} uni_s;

typedef union Universal_Multi_U {
	
	/**
     * Union to store commonly used types and array pointers.
     */
	
	bool       b;
	
	int32_t    i; 
	int32_t *  ii;
	int32_t ** iii;
	
	float      f;
	float   *  ff;
	float   ** fff;
	
	char       c;
	char    *  s;
	char    ** ss;
	
} multi_u;

typedef struct Universal_Multi_S {
	
	/**
     * Structure to store commonly used types and array pointers, 
	 * as well as their types and lengths.
     */

	multi_u  value  ;
	type_e   type   ;
	int32_t  length ;
	int32_t* lengths;	
} multi_s;

size_t getSizeOfType(
	const type_e type
	) {
	
	/**
     * Return the size, in bytes, of entered type enum.
     * @param 
     *     const type_e type: type enum to return size of.
     * @see
     * @return size_t size: size of type.
     */
	
	size_t size = 0;
	switch(type) {
		
		case(bool_e        ): size = sizeof(bool      ); break;
		
		case(int_e         ): size = sizeof(int32_t   ); break;
		case(int_array_e   ): size = sizeof(int32_t  *); break;
		case(int_jagged_e  ): size = sizeof(int32_t **); break;			
			
		case(float_e       ): size = sizeof(float     ); break;
		case(float_array_e ): size = sizeof(float    *); break;
		case(float_jagged_e): size = sizeof(float   **); break;		

		case(char_e        ): size = sizeof(char      ); break;
		case(string_e      ): size = sizeof(char     *); break;		
		case(string_array_e): size = sizeof(char    **); break;
			
		default:
			
			fprintf(
				stderr, 
				"Warning! Type \"%i\" not recognised! \n", 
				type
			);
		break;
	}
	
	return size;
}

bool isTypeInt(
    const type_e type
    ) {
	
	/**
     * Checks if type is an integer.
     * @param 
     *     const type_e type: enum holding type infomation.
     * @see
     * @return bool is_int: 1 if type is int, else 0.
     */
	
	bool is_int = false;
	
	switch(type) {
		
		case(bool_e        ): is_int = true ; break;
		
		case(int_e         ): is_int = true ; break;
		case(int_array_e   ): is_int = true ; break;
		case(int_jagged_e  ): is_int = true ; break;			
			
		case(float_e       ): is_int = false; break;
		case(float_array_e ): is_int = false; break;
		case(float_jagged_e): is_int = false; break;		

		case(char_e        ): is_int = false; break;
		case(string_e      ): is_int = false; break;		
		case(string_array_e): is_int = false; break;
	}
	
	return is_int;
}

char *typetoString(
	const type_e type
	) {
	
	/**
     * Convert type value to string.
     * @param 
     *     const type type: type_e enum to return string name.
     * @see
     * @return char *string: name of type as string.
     */
	
	char *string = NULL;
	switch(type) {
		
		case(bool_e        ): string = "bool"              ; break;
		
		case(int_e         ): string = "int"               ; break;
		case(int_array_e   ): string = "int array"         ; break;
		case(int_jagged_e  ): string = "jagged int array"  ; break;			
			
		case(float_e       ): string = "float"             ; break;
		case(float_array_e ): string = "float array"       ; break;
		case(float_jagged_e): string = "jagged float array"; break;		

		case(char_e        ): string = "char"              ; break;
		case(string_e      ): string = "string"            ; break;		
		case(string_array_e): string = "string array"      ; break;
			
		default:
			
			fprintf(stderr, "Warning! Type \"%i\" not recognised! \n", type);
		break;
	}
	
	return string;
}

multi_s StringToMultiS(
	const char*  string,
	const type_e type
	) {
	
	/**
     * Convert string to void pointer with structure of type.
     * @param 
     *     const multi_s data: multi_s structure to return in string format.
     * @see
     * @return char *string: inputted multi_s value in string format.
     */
	multi_s value;
	value.type = type;
	
	switch(type) {
		
		case(bool_e  ): 
			if isdigit(string[0]) {
				int32_t bool_value = atoi(string);
				
				if ((bool_value > 1) || (bool_value < 0)) {
				
					fprintf(stderr, "Warning! Bool value %s not recognised! \n", string);
					value.value.b = false;
				} else {
				
					value.value.b = (bool) atoi(string); 
				}
				
			} else if (!strcmp(string, "true")) {
			
				value.value.b = true;
			} else if (!strcmp(string, "false")) {
			
				value.value.b = false;
			} else {
				fprintf(stderr, "Warning! Bool value %s not recognised! \n", string);
				value.value.b = false;
			}
		break;
		case(int_e   ): 
			if isdigit(string[0]) {
			
				value.value.i = (int32_t) atoi(string); 
			} else {
			
				fprintf(stderr, "Warning! Int value %s not recognised! \n", string);
				value.value.i = 0;
			}
			break;
		case(float_e ):
			if isdigit(string[0]) {
			
				value.value.f = (float) atof(string); 
			} else {
			
				fprintf(stderr, "Warning! Float value %s not recognised! \n", string);
				value.value.f = 0.0f;
			}
			break;
		case(char_e  ): value.value.c = (char   ) string[0]   ; break;
		case(string_e): asprintf(&value.value.s, "%s", string); break;		
	
		default:
			fprintf(stderr, "Warning! Type \"%i\" not recognised! \n", type);
			value.value.f = 0.0f;
		break;
	}
	
	return value;
}

char *MultiStoString(
	const multi_s data
	) {
	
	/**
     * Convert multi_s value to string.
     * @param 
     *     const multi_s data: multi_s structure to return in string format.
     * @see
     * @return char *string: inputted multi_s value in string format.
     */
	
	char* string;
	switch(data.type) {
		
		case(bool_e        ): asprintf(&string, "%i", (int)    data.value.b  ); break;
		
		case(int_e         ): asprintf(&string, "%i",          data.value.i  ); break;
		case(int_array_e   ): asprintf(&string, "%p", (void *) data.value.ii ); break;
		case(int_jagged_e  ): asprintf(&string, "%p", (void *) data.value.iii); break;			
			
		case(float_e       ): asprintf(&string, "%f",          data.value.f  ); break;
		case(float_array_e ): asprintf(&string, "%p", (void *) data.value.ff ); break;
		case(float_jagged_e): asprintf(&string, "%p", (void *) data.value.fff); break;		

		case(char_e        ): asprintf(&string, "%c", data.value.c  ); break;
		case(string_e      ): string = data.value.s; break;		
		case(string_array_e): asprintf(&string, "%p", (void *) data.value.ss ); break;
			
		default:
			
			fprintf(stderr, "Warning! Type \"%i\" not recognised! \n", data.type);
		break;
	}
	
	return string;
}

void* allocateTypeArray(
	const type_e  type, 
	const int32_t num_elements
    ) {
	
	/**
     * Allocate array of size of type with num_elements.
     * @param 
     *     const type_e  type        : type to allococate as array elements.
	 *     const int32_t num_elements: num_elements to allocate.
     * @see
     * @return void *array: pointer to newly assigned memory.
     */
	
	return malloc(getSizeOfType(type) * (size_t) num_elements);
}

void freeMultiS(
	multi_s data
	) {
	
	/**
     * Free memory assigned to multi_s structure.
     * @param 
     *     const multi_s data: multi_s structure to return in string format.
     * @see
     * @return none.
     */
	
	switch(data.type) {
		
		case(int_array_e   ): if (data.value.ii  != NULL) {free(data.value.ii); } break;
		case(float_array_e ): if (data.value.ff  != NULL) {free(data.value.ff); } break;
		case(string_array_e): if (data.value.ss  != NULL) {free(data.value.ss); } break;
		case(int_jagged_e  ): 
		if (data.value.iii != NULL) {
				
			for (int32_t index = 0; index < data.length; index++) {
					
					if (data.value.iii[index] != NULL) {
						
						free(data.value.iii[index]);
					}
					
					if (data.lengths != NULL) {
						
						free(data.lengths);
					}
                    
				}
                free(data.value.iii);
			} 
		break;
			
		case(float_jagged_e):
			
			if (data.value.fff != NULL) {
				
				for (int32_t index = 0; index < data.length; index++) {
					
					if (data.value.fff[index] != NULL) {
						
						free(data.value.fff[index]);
					}
					
				}
                
                if (data.lengths != NULL) {
						
					free(data.lengths);
				}
                
                free(data.value.fff);
			} 
		break;
		
		default:
			
			fprintf(stderr, "Warning! Type \"%i\" not recognised! \n", data.type);
		break;
	}
}

typedef struct DictEntry {
	
	      multi_s     data;   
	
          int32_t     hash; 
    const char       *string_key;
	
	struct DictEntry *previous_entry;
	struct DictEntry *next_entry;
	
} dict_entry_s;

typedef struct Dictionary {
	
	int32_t        length;
	int32_t        num_entries;
	
	dict_entry_s **entries;
	dict_entry_s  *last_entry;
	
} dict_s;

dict_s* makeDictionary(
	const int32_t length
	) {
	
	/**
     * Create dictionary data structure.
     * @param 
     *     const int32_t length: number of elements to allocate for storage of unique hashes.
     * @see
     * @return dict_s *dictionary: pointer to newly created dictionary.
     */
	
	dict_s* dict      = malloc(sizeof(dict_s));
	
	dict->length      = length;
	dict->num_entries = 0;
	dict->entries     = calloc( (size_t) length, sizeof(dict_entry_s*));
	dict->last_entry  = NULL;
	
	return dict;
}

int32_t StringToKey(
	const char *string_key
	) {
	
	/**
     * Generate pseudo-random number from string.
     * @param 
     *     const char string_key: String key to convert to numerical key.
     * @see getDictHashCode()
     * @return int32_t key: pseudo random numerical key.
     */
	
	int     index   = 0;
	char    current = string_key[index];
	int32_t key     = 0;
	
	while (current != '\0') {
		
		key += (int) current*index;	
		index++;
		
		current = string_key[index];
	}
	return key;
}

int getDictHashCode(
	const dict_s *dict      , 
	const char   *string_key
	) {
	
	/**
     * Find dict hash given string key input. Will
	 * always return same hash for the same string,
	 * but Dose not garantee different hash for 
	 * differnt strings.
     * @param 
	 *     const dict_s *dict     : Dictionary to hash with string_key.
     *     const char  *string_key: String key to link to dictionary index.
     * @see StringToKey()
     * @return int32_t hash: hash to dictionary index.
     */

   int32_t key = StringToKey(string_key);
   return key % dict->length;
}

int32_t insertDictEntry(
	      dict_s  *dict      , 
	const multi_s  data      , 
    const char    *string_key
	) {
	
	/**
     * Insert new multi_s entry into dictionary pointed to by dict.
     * @param 
	 *     const dict_s  *dict      : Dictionary to hash with string_key.
	 *     const multi_s *data      : Data to insert into dictionary.
     *     const char    *string_key: String key to insert data at given hash.
     * @see getDictHashCode(), multi_s.
     * @return int32_t return_value: Returns 0 if successful, else return 1.
     */

	dict->num_entries++;
	if (dict->num_entries > dict->length) {
		
		fprintf(stderr, "Warning dictionary too small! Cannot add entry: %s. \n", string_key);
		return 1;
	}
	
	const int32_t hash = getDictHashCode(dict, string_key);
	
	dict_entry_s* entry = malloc(sizeof(dict_entry_s));
	
	*entry = (dict_entry_s) {  
		data            ,
		hash            ,
		string_key      ,
		dict->last_entry,
		NULL
	};
		
	if ( dict->last_entry != NULL) {
		
		dict->last_entry->next_entry = entry;
	}
		
	int32_t index = hash;
	while (dict->entries[index] != NULL){
		
		index++; index %= dict->length; //Wrap around. 
	}
		
	dict->entries[index] = entry;
	dict->last_entry     = entry;
	
	return 0;
}

int32_t findHashIndex(
	const dict_s *dict, 
	const char   *string_key
	) {
	
	/**
     * Find dictionary entry index from entered string key.
     * @param 
	 *     const dict_s  *dict      : Dictionary to find hash for specified string key;.
     *     const char    *string_key: String key to find hash index from.
     * @see getDictHashCode().
     * @return int32_t index: Returns hash index if successful, else return -1.
     */
	 
	const int32_t hash = getDictHashCode(dict, string_key);
	      int32_t index = hash;
	
	const int32_t stop_index = ((hash - 1) < 0) ? (dict->length - 1) : (hash - 1);
	
	while (dict->entries[index] != NULL) {
		
		if (!strcmp(dict->entries[index]->string_key, string_key)) {
			
			return index;
		} else if (index == stop_index) {
			
			fprintf(stderr, "Warning! Cannot find entry! %s. \n", string_key);
			return -1;		
		}
			
		index++; index %= dict->length; //Wrap around. 
	}
	
	fprintf(stderr, "Warning! Cannot find entry! %s. \n", string_key);
	return -1;
}

dict_entry_s *findDictEntry(
	const dict_s *dict    , 
	const char   *string_key
	) {
	
	/**
     * Find dictionary entry from entered string key.
     * @param 
	 *     const dict_s  *dict      : Dictionary to find entry for specified string key.
     *     const char    *string_key: String key to find dictionary entry of.
     * @see findHashIndex().
     * @return dict_entry_s entry: Returns dict entry if successful, else return NULL.
     */
	
	const int32_t index = findHashIndex(dict, string_key);
	
	if (index >= 0) {
		
		return dict->entries[index];
	} else {
		
		return NULL;
	}
}

void deleteDictEntry(
	        dict_s * dict      ,
	const   char   * string_key
	) {
	
	/**
     * Delete dictionary entry at entered string key.
     * @param 
	 *     const dict_s  *dict      : Dictionary to find entry for specified string key.
     *     const char    *string_key: String key to delete dictionary entry at.
     * @see findHashIndex().
     * @return none.
     */
	
	const int32_t index = findHashIndex(dict, string_key);
	
	if (index >= 0) {
		
		if (dict->entries[index]->previous_entry != NULL) {
			if (dict->entries[index]->next_entry != NULL) {

				dict->entries[index]->previous_entry->next_entry = dict->entries[index]->next_entry;
				dict->entries[index]->next_entry->previous_entry = dict->entries[index]->previous_entry;

			} else {

				dict->entries[index]->previous_entry->next_entry = NULL;
				dict->last_entry = dict->entries[index]->previous_entry;
			}
		} 
		
		dict->num_entries--;
		free(dict->entries[index]);
        
        dict->entries[index] = NULL;
	}
}

dict_entry_s** returnAllEntries(
	const dict_s* dict
	) {
	
	/**
     * Returns array of all dictionary entries
     * @param 
	 *     const dict_s  *dict: Dictionary to find entry for specified string key.
     * @see findHashIndex().
     * @return dict_entry_s** entries: array of dict entries.
     */
	
	dict_entry_s** entries = malloc(1);
	if ((dict->num_entries > 0) && (dict->last_entry != NULL)) {
		
		entries = realloc(entries, sizeof(dict_entry_s*) * (size_t) dict->num_entries);
		
		entries[dict->num_entries - 1] = dict->last_entry;
		for (int32_t index = dict->num_entries - 2; index >= 0; index--) {
			
			entries[index] = entries[index + 1]->previous_entry;
		}
	}
	
	return entries;
}

void printDictKeys(
	const dict_s* dict
	) {
	
	/**
     * Print all dictionary keys.
     * @param 
	 *     const dict_s  *dict: Dictionary to print all keys.
     * @see returnAllEntries().
     * @return none
     */
	
	dict_entry_s** entries = returnAllEntries(dict);

	for (int32_t index = 0; index < dict->num_entries; index++) {
		
		printf("%s \n", entries[index]->string_key);	
	}
}

void freeDictEntry(
	dict_entry_s* entry
	) {	
	
	/**
     * Deallocate memory assigned to dictinary entry
     * @param 
	 *     const dict_entry_s  *entry: Dictionary entry to deallocate.
     * @see freeMultiS().
     * @return none
     */
	
	freeMultiS(entry->data);
	free(entry);
}

void freeDictionary(
	dict_s* dict
	) {
	
	/**
     * Deallocate all memory assigned to dictinary and its entry.
     * @param 
	 *     const dict_s  *dict      : Dictionary entry to deallocate.
     * @see freeMultiS().
     * @return none
     */
	
	dict_entry_s** entries = returnAllEntries(dict);
	
	for (int32_t index = 0; index < dict->num_entries; index++) {
	
		freeDictEntry(entries[index]);
	}
	
	free(entries);
	free(dict->entries);
	free(dict);
}

typedef struct Mapping {

	/**
     * Structure to hold map to convert string to index.
     */
    
    dict_s   * dict;
    char    ** keys;    
    int32_t    length;
} map_s;

typedef struct Map_Pairing {
	
	/**
	 * Structure to hold index key pair.
	 */
    
    char    *  key;
    int32_t    index;
    map_s      map;
} map_pair_s;

map_s createMap(
          char    ** keys    , 
    const int32_t    num_keys
    ) {
	
	/**
     * Create map between array of strings and array of ints
     * @param 
	 *           char    ** keys    : Strings to map.
	 *     const int32_t    num_keys: Number of strings to map.
     * @see makeDictionary(), insertDictEntry().
     * @return map_s map: map between keys and index.
     */
	
    
    map_s map;
    map.dict = makeDictionary(10*num_keys);
    
    for (int32_t index = 0; index < num_keys; index++) {
        
        const multi_s data = (multi_s) {*((multi_u*)(&index)), int_e, 1, NULL};
        insertDictEntry(map.dict, data, keys[index]);
	}   
    
    map.keys   = keys;
    map.length = num_keys;
    
    return map;
}

int32_t getMapIndex(
        const map_s   map, 
        const char   *key
    ) {
	
	/**
     * Return index of key in map.
     * @param 
	 *     const map_s  map: Key to index map.
	 *     const char  *key: String to find index of in map.
     * @see findDictEntry(), createMap().
     * @return int32_t index: Index corresponding to inputted key.
     */
    
    return findDictEntry(map.dict, key)->data.value.i;
}

char *getMapKey(
    const map_s   map,
    const int32_t index
    ) {
	
	/**
     * Find string corresponding to inputted index
     * @param 
	 *     const map_s   map: Key to index map.
	 *     const int32_t index: Index to find correspinding string in map.
     * @see findDictEntry(), createMap().
     * @return char *key: String_key corresponding to inputted index.
     */
    
    return map.keys[index];
}
    
#endif