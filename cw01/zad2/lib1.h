#ifndef lib1_h__
#define lib1_h__

extern struct block* allocate_array(int n);
extern void deallocate_array(struct block* array,int n);
extern void allocate_block(struct block* array, int n, int i);
extern void deallocate_block(struct block* array, int i);
extern int dist(int a, int b);
extern int find(struct block* array, int n, int number);
extern int insert_to_block(struct block* array, int id, char text[], int text_size);
extern void read_block(struct block* array, int id);

#endif
