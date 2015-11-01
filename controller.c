//controller.c
#include <stdio.h> 
#include <fcntl.h> 
#include <stdlib.h>
#include <string.h>


int iif[90], oif[90];	//max number of iff in a topo file is 90
int num_lines;			//# lines in topofile
int topo[10][10];		// if a certain channel is in topo file

int num_read_output[10], fd_output[10], oflag_output[10], fd_input[10], oflag_input[10];
char buf_out_0[1000], buf_out_1[1000], buf_out_2[1000], buf_out_3[1000], buf_out_4[1000];
char buf_out_5[1000], buf_out_6[1000], buf_out_7[1000], buf_out_8[1000], buf_out_9[1000];	//buffers storing read from output

void read_topology();   // read topology and set iff and oif
void open_files();      // file initializations
void pass_messages(); 	// pass hello and other messages


int main(){


	printf("this program takes about 150 seconds to finish, please remain patient :)\n");
	int i, j;
	for( i = 0; i<10; i++){
		for( j = 0; j<10; j++){
			topo[i][j] = 0;
		}
	}
	read_topology();
	open_files();
	
	sleep(1);
	int timer;
	for ( timer = 0; timer < 200; timer++){
    	
        pass_messages();//pass messages every 1 second
        
        sleep(1);
        
  	}
		
	return 0;

}

void read_topology(){

	int fd_topo, num_read_topo, oflag_topo;
	char buf_topo[1000];

	oflag_topo = (O_CREAT | O_RDONLY);
	fd_topo = open("topology.txt", oflag_topo, 0x1c0);
	if (fd_topo < 0 ) {printf("Error opening\n"); exit(1);}

	num_read_topo = read(fd_topo, buf_topo, 1000); 
	//printf("reader: # bytes read %d which were: %s\n", num_read_topo, buf_topo);

	num_lines = 0;
	int i;
	for( i = 0; i < num_read_topo; i += 4){
		iif[num_lines] = atoi(&buf_topo[i]);
		oif[num_lines] = atoi(&buf_topo[i+2]);
		topo[atoi(&buf_topo[i])][atoi(&buf_topo[i+2])] = 1;
		num_lines++;		
	}

	return;
}

//need to pass all messages here

void pass_messages(){


    num_read_output[0] = read(fd_output[0], buf_out_0, 1000);    //message is now in buffer
    num_read_output[1] = read(fd_output[1], buf_out_1, 1000);    //message is now in buffer
    num_read_output[2] = read(fd_output[2], buf_out_2, 1000);    //message is now in buffer
    num_read_output[3] = read(fd_output[3], buf_out_3, 1000);    //message is now in buffer
    num_read_output[4] = read(fd_output[4], buf_out_4, 1000);    //message is now in buffer
    num_read_output[5] = read(fd_output[5], buf_out_5, 1000);    //message is now in buffer
    num_read_output[6] = read(fd_output[6], buf_out_6, 1000);    //message is now in buffer
    num_read_output[7] = read(fd_output[7], buf_out_7, 1000);    //message is now in buffer
    num_read_output[8] = read(fd_output[8], buf_out_8, 1000);    //message is now in buffer
    num_read_output[9] = read(fd_output[9], buf_out_9, 1000);    //message is now in buffer
	
	
	int j; 
    for( j=0; j<10; j++){
    	if(topo[0][j] == 1){
    		write(fd_input[j], buf_out_0, num_read_output[0]);
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[1][j] == 1){
    		write(fd_input[j], buf_out_1, num_read_output[1]); //
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[2][j] == 1){
    		write(fd_input[j], buf_out_2, num_read_output[2]); //
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[3][j] == 1){
    		write(fd_input[j], buf_out_3, num_read_output[3]); //
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[4][j] == 1){
    		write(fd_input[j], buf_out_4, num_read_output[4]); //
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[5][j] == 1){
    		write(fd_input[j], buf_out_5, num_read_output[5]); //
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[6][j] == 1){
    		write(fd_input[j], buf_out_6, num_read_output[6]); //
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[7][j] == 1){
    		write(fd_input[j], buf_out_7, num_read_output[7]); //
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[8][j] == 1){
    		write(fd_input[j], buf_out_8, num_read_output[8]); //
    	}
    }
    
    for( j=0; j<10; j++){
    	if(topo[9][j] == 1){
    		write(fd_input[j], buf_out_9, num_read_output[9]); //
    	}
    }
}

void open_files(){

	//file openning etc...
    oflag_output[0] = ( O_CREAT | O_RDONLY );
    fd_output[0] = open("output_0.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[0] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[0] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[0] = open("input_0.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[1] = ( O_CREAT | O_RDONLY );
    fd_output[1] = open("output_1.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[1] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[1] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[1] = open("input_1.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[2] = ( O_CREAT | O_RDONLY );
    fd_output[2] = open("output_2.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[2] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[2] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[2] = open("input_2.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[3] = ( O_CREAT | O_RDONLY );
    fd_output[3] = open("output_3.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[3] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[3] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[3] = open("input_3.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[4] = ( O_CREAT | O_RDONLY );
    fd_output[4] = open("output_4.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[4] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[4] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[4] = open("input_4.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[5] = ( O_CREAT | O_RDONLY );
    fd_output[5] = open("output_5.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[5] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[5] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[5] = open("input_5.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[6] = ( O_CREAT | O_RDONLY );
    fd_output[6] = open("output_6.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[6] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[6] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[6] = open("input_6.txt",O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[7] = ( O_CREAT | O_RDONLY );
    fd_output[7] = open("output_7.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[7] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[7] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[7] = open("input_7.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[8] = ( O_CREAT | O_RDONLY );
    fd_output[8] = open("output_8.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[8] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[8] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[8] = open("input_8.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
    //file openning etc...
    oflag_output[9] = ( O_CREAT | O_RDONLY );
    fd_output[9] = open("output_9.txt", O_RDONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR); //not neccessary output_0, depends on iif
    if (fd_output[9] < 0 ) {printf("Error opening\n"); exit(1);}
    
    oflag_input[9] = (O_TRUNC | O_CREAT | O_WRONLY);		//open/creat input files
    fd_input[9] = open("input_9.txt", O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);

    return;
}




