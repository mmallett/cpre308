/*
    Matt Mallett
    CprE 308 Project 2
    life.c
    main implementation file for Conway's Threaded Game of Life

    DEFAULT VALUES OF PARAMETERS
    if no command line argument is given, these values will be defaulted to:
    grid size   4
    num threads 1
    num gen     2
    input       stdin
    output      stdout
*/

#include "life.h"

//threading variables


void* t_simulate(void*);

pthread_mutex_t thread_complete_mut;
pthread_cond_t  thread_complete_cv;
int             threads_running;

pthread_mutex_t gen_complete_mut;
pthread_cond_t  gen_complete_cv;
int             gen_complete;

//thread functions, simulate life, and synchronize at start of generation
void*
t_simulate(void* arg)
{
    int
    gen, *px = gen_x;
    arg_t* input = (arg_t*) arg;    

    for(gen = 0; gen < num_gens; gen++)
    {
        //wait on main
        pthread_mutex_lock(&gen_complete_mut);
        while(gen > gen_complete)
        {
            pthread_cond_wait(&gen_complete_cv,&gen_complete_mut);
        }
        pthread_mutex_unlock(&gen_complete_mut);  
        int i;
        for(i=input->start;i<=input->end;i++)
        {
            int j = i/grid_size;
            int k = i%grid_size;
            *(px+i) = evolve(j,k);
        }
        //lock
        pthread_mutex_lock(&thread_complete_mut);
        threads_running--;
        //broadcast
        pthread_cond_broadcast(&thread_complete_cv);
        pthread_mutex_unlock(&thread_complete_mut);     
    }
}

//perform same duties as thread function, and push program to next generation
void*
main_simulate(int start, int end)
{
    int
    gen, *px = gen_x;

    for(gen = 0; gen < num_gens; gen++)
    {
        int i;
        for(i=start;i<=end;i++)
        {
            int j = i/grid_size;
            int k = i%grid_size;
            *(px+i) = evolve(j,k);
        }
        //lock
        pthread_mutex_lock(&thread_complete_mut);
        threads_running--;
        //broadcast
        pthread_cond_broadcast(&thread_complete_cv);
        while(threads_running != 0)
        {
            //wait
            pthread_cond_wait(&thread_complete_cv,&thread_complete_mut);
        }
        threads_running = num_threads;
        pthread_mutex_unlock(&thread_complete_mut);

        memcpy(gen_y,gen_x,grid_size*grid_size*sizeof(int));
        pthread_mutex_lock(&gen_complete_mut);
        gen_complete++;
        //broadcast
        pthread_mutex_unlock(&gen_complete_mut);
        pthread_cond_broadcast(&gen_complete_cv);        
    }
}

int
main(int argc, char ** argv)
{
    // handle command line arguments
    int
    n_flag, t_flag, r_flag, i_flag, o_flag, c;

    input = (char*) malloc(100 * sizeof(char));
    output= (char*) malloc(100 * sizeof(char));
    
    while ((c = getopt (argc, argv, "n:i:t:r:o:")) != -1)
    {
        switch (c)
        {
            char* buffer;
            case 'n':
                n_flag = 1;
                buffer = strdup(optarg);
                sscanf(buffer,"%d",&grid_size);
                free(buffer);
                break;
            case 't':
                t_flag = 1;
                buffer = strdup(optarg);
                sscanf(buffer,"%d",&num_threads);
                free(buffer);
                break;
            case 'r':
                r_flag = 1;
                buffer = strdup(optarg);
                sscanf(buffer,"%d",&num_gens);
                free(buffer);
                break;
            case 'i':
                i_flag = 1;
                sprintf(input,"%s",optarg);
                break;
            case 'o':
                o_flag = 1;
                sprintf(output,"%s",optarg);
                break;
            case '?':
                printf("?\n");
                break;
            default:
                break;
                //abort ();
        }
    }
    
    //set defaults for values not defined in args
    if(!n_flag) grid_size   = DEFAULT_SIZE;
    if(!t_flag) num_threads = DEFAULT_THREADS;
    if(!r_flag) num_gens    = DEFAULT_GENS;
    if(!i_flag) input       = NULL;
    if(!o_flag) output      = NULL;

    //check bad inputs
    if(grid_size < 0)
    {
        printf("grid size must be >= 0\nexiting\n");
        exit(1);
    }
    if(num_threads < 1)
    {
        printf("num threads must be >= 1\nexiting\n");
        exit(1);
    }
    if(num_gens < 0)
    {
        printf("num gens must be >= 0\nexiting\n");
        exit(1);
    }

    //allocate generation memory
    gen_x = (int*) malloc(grid_size * grid_size * sizeof(int));
    gen_y = (int*) malloc(grid_size * grid_size * sizeof(int));
    
    init_gen_x();
    memcpy(gen_y,gen_x,grid_size*grid_size*sizeof(int));
    
    //distribute thread workload
    arg_t arg_list[num_threads];
    int start = 0;
    int total=grid_size*grid_size;
    int step = total/num_threads;
    int extra = total%num_threads;
    int i;
    for(i=0;i<num_threads-1;i++)
    {
        arg_list[i].start=start;
        start += step;
        arg_list[i].end = start - 1;
    }
    int main_start = start;
    int main_end = start + step + extra -1;

    //initialize thread variables
    pthread_mutex_init(&thread_complete_mut,NULL);
    pthread_mutex_init(&gen_complete_mut,NULL);
    pthread_cond_init(&thread_complete_cv,NULL);
    pthread_cond_init(&gen_complete_cv,NULL);
    
    gen_complete = 0;
    threads_running = num_threads;

    //spin up threads
    pthread_t threads[num_threads-1];
    for(i=0;i<num_threads-1;i++)
    {
        pthread_create(&threads[i],NULL,t_simulate,(void*)&arg_list[i]);
    }
    //run main thread
    main_simulate(main_start,main_end);
    
    //synchronize before exiting
    for(i=0;i<num_threads-1;i++)
    {
        pthread_join(threads[i],NULL);
    }
        
    //output
    dump_gen_x();
}

/*
read input matrix either from stdin or specified file
stores it in gen_x
*/
void
init_gen_x()
{
    int* ptr = gen_x;
    if(input == NULL)
    {
        int i,j;
        for(i=0;i<grid_size;i++)
        {
            for(j=0;j<grid_size;j++)
            {
                printf("Enter value for gen1[%d][%d]\n",i,j);
                int tmp;
                scanf("%d", &tmp);
                if(tmp != 0 && tmp != 1)
                {
                    printf("Bad input\nexiting..\n");
                    exit(1);
                }
                *(ptr++) = (tmp)? '1':'0';
            }
        }

    }
    else
    {
        FILE*
        read_file = fopen(input,"r");
        if(read_file == NULL)
        {
            perror("Could not read file");
            exit(1);
        }
        int
        i=0, dec = grid_size * grid_size, c;
        while(c != EOF)
        {
            c = fgetc(read_file);
            if(c == '0' || c == '1')
            {
                *(ptr+i++) = c;
                dec--;
            }
        }
        if(dec != 0)
        {
            printf("Incorrectly formatted file\nexiting..\n");
            exit(1);
        }  
    }
}

/*
outputs gen_x matrix to either stdout or specified file
*/
void
dump_gen_x()
{
    FILE* out_file;
    if(output == NULL)
    {
        int i;
        int* ptr = gen_x;
        for(i=0;i<grid_size*grid_size;i++)
        {
            if(i%grid_size == 0 && i != 0) fprintf(stdout,"\n");
            fprintf(stdout,"%c ",*(ptr+i));
        }
        fprintf(stdout,"\n");
    }    
    else
    {
        out_file = fopen(output,"w");
        if(out_file == NULL)
        {
            perror("Could not open output file");
            exit(1);
        }
        int i;
        int* ptr = gen_x;
        for(i=0;i<grid_size*grid_size;i++)
        {
            if(i%grid_size == 0 && i != 0) fprintf(out_file,"\n");
            fprintf(out_file,"%c ",*(ptr+i));
        }
        fprintf(out_file,"\n");
        fclose(out_file);
    }
}

/*
determines the state of the (i,j) element in gen_x by
checking its state in gen_y
*/
int
evolve(int i, int j)
{
    int
    living_neighbors = 0, index;

    int* py = gen_y;

    int cur = (*(py+get_index(i,j)) == '1') ? 1:0;
    
    //check up left       i-1 j-1
    if(i != 0 && j != 0)
    {
        index = get_index(i-1,j-1);
        if(*(py+index) == '1')
            living_neighbors++;
    }
    //check up            i-1    
    if(i != 0)
    {
        index = get_index(i-1,j);
        if(*(py+index) == '1')
            living_neighbors++;
    }
    //check up right      i-1 j+1
    if(i != 0 && j != grid_size-1)
    {
        index = get_index(i-1,j+1);
        if(*(py+index) == '1')
            living_neighbors++;
    }
    //check left          j-1
    if(j != 0)
    {
        index = get_index(i,j-1);
        if(*(py+index) == '1')
            living_neighbors++;
    }
    //check right         j+1
    if(j != grid_size-1)
    {
        index = get_index(i,j+1);
        if(*(py+index) == '1')
            living_neighbors++;
    }
    //check down left     j-1 i+1
    if(i != grid_size-1 && j != 0)
    {
        index = get_index(i+1,j-1);
        if(*(py+index) == '1')
            living_neighbors++;
    }
    //check down          i+1
    if(i != grid_size-1)
    {
        index = get_index(i+1,j);
        if(*(py+index) == '1')
            living_neighbors++;
    }
    //check down right    i+i j+1
    if(i != grid_size-1 && j != grid_size-1)
    {
        index = get_index(i+1,j+1);
        if(*(py+index) == '1')
            living_neighbors++;
    }

    int ret = (!cur) ? living_neighbors == 3:
        living_neighbors > 1 && living_neighbors < 4;

    return (ret) ? '1':'0';
}

/*
convert ordered pair to pointer offset
*/
int get_index(int i, int j)
{
    return i * grid_size + j;
}





 
