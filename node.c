//node.c
#include <stdio.h> 
#include <fcntl.h> 
#include <stdlib.h>
#include <string.h>
#define INF 900;


int fdo,fdi;//channel files id
int fd_rec; //r_received_from_s.txt id
int id_node;
int num_read_input;
int timer = 0;  //node timer
char read_buffer[10000];
int lsa_ts = 0; //timestamp for lsa
int is_receiver = 0; //flag for receiver or and sender
int is_sender = 0;
int id_sender = -1; 
int temp_root_tree = -1; //record sender on the tree
int temp_child_tree = -1;	//record child on the tree
int temp_parent_tree = -1; // what if this node involves in more than one tree???


int hello_from[10];    // store hello node index only!, also works for parent identification!
char *linkstate_database[10]; //database to store lsa
int linkstate_mat[10][10];
int linkstate_time[10];

char data_temp_buffer[200];

int data_parent[10];
int data_child[10];

int join_time[10][10];
int join_mat[10][10];


void creat_channels(int id);
void send_hello();
void send_lsa();
void process_inputs();// read inputs and process each new message received
int shortest_path(int a, int b, int *path); //return the number of nodes on the path
int send_join(int);
void send_data(); // send data if i am the sender
void clear_linkstate();

int main(int argc, char *argv[]) {

	//printf("id of the node: %d \n", id_node);
  switch (argc) {
    //normal node
    case 2:
    //printf("this node is neither a sender nor a receiver \n");
    id_node = atoi(argv[1]);
    //printf("id of the node: %d \n", id_node);
    creat_channels(id_node);
    break;

    case 4:
    //either a sender or a receiver
    if(strcmp(argv[2],"receiver") == 0){
		
     // printf("this node is a receiver \n");
	  is_receiver = 1;
      id_node = atoi(argv[1]);
      //printf("id of the node: %d \n", id_node);
      creat_channels(id_node);
	  //get sender:
	  id_sender = atoi(argv[3]); 
	  //need to creat a file "r_received_from_s.txt".....:
		int oflag_rec;
		char file_name_rec[]="0_received_from_0.txt";
	  
		file_name_rec[0] += (char)id_node;
		file_name_rec[16] += (char)id_sender;
	  
		oflag_rec = (O_TRUNC | O_CREAT | O_WRONLY);
		fd_rec = open(file_name_rec, oflag_rec, 0x1c0);
		if (fd_rec < 0 ) {printf("Error opening\n"); exit(1);}
		  

    }else if(strcmp(argv[2],"sender") == 0){
	
	  char sender_message[100]; //store the message if it is a sender
	  //memset(sender_message,'\0',100);
	  int len_message = 0; //store the length of the message sent by the sender!
      //printf("this node is a sender \n");
	  is_sender = 1;
      id_node = atoi(argv[1]);
      //printf("id of the node: %d \n", id_node);
      creat_channels(id_node);
	  //now need to store the message sent by the sender somewhere:
	  len_message = strlen(argv[3]);
	  strncpy(sender_message, argv[3], len_message); 
	  //sender_message waiting to be concatenated to "data id_node, id_root xxxxxxx"
	  
	  memset(data_temp_buffer,'\0',200);
	  strncpy(data_temp_buffer, "data 0 0 ", 9);
	  strcat(sender_message,"\n");
	  strcat(data_temp_buffer, sender_message);
	  //printf("the message to be sent: %s ", data_temp_buffer);
	  data_temp_buffer[5] += id_node; 
	  data_temp_buffer[7] += id_node; 
	  //the message is now in data_temp_buffer..
	  
	  
    }else{
      printf("wrong arguments input for node.c, pls re-enter arguments \n");
      //return 0;
    }
    break;

    default:
    printf("wrong arguments input for node.c, pls re-enter arguments \n");
    break;
}

	memset(hello_from,-1,10); // initialize hello_from for future use!
	int k;
	for(k=0; k<10; k++)
	{
		linkstate_database[k]= -1;//(char*)malloc(33*sizeof(char));
	}

	int m, n;
	for(m=0; m<10; m++){
		linkstate_time[m]=-1;
		data_parent[m]=-1;	//data_parent[sender] ==> parent on the path
		data_child[m]=-1;	//data_child[sender] ==> child on the path
		for(n=0; n<10; n++)
		{
			join_time[m][n]=-1;
			join_mat[m][n]=-1;
		}
		clear_linkstate(m);
		hello_from[m]=-1;
	}
	
	int i;
	for ( i = 0; i < 150; i++){
 
    if (i%5 == 0){
      send_hello(); //hello every 5 seconds
    }

    sleep(1);
	process_inputs();

    if((i+1)%10 == 0){
      send_lsa();
	  if(is_sender) send_data();
    }
	if( (i>=20) && (i%10 == 0) && is_receiver){
      //data_parent[id_sender]=send_join(id_sender);
	  join_mat[id_node][id_sender]=send_join(id_sender);
	  join_time[id_node][id_sender]=20;
    }
	
	int j=0,k=0;
	for(j=0; j<10; j++)
	{
		linkstate_time[j]--;
		hello_from[j]--;
		if(linkstate_time[j]<0)	
		{
			clear_linkstate(j);
		}
		for(k=0; k<10; k++)
		{
			join_time[k][j]--;
			if(join_time[k][j]<0)
			{
				join_mat[k][j]=-1;
			}
		}
	}
    timer++;
  }


  return 0;
}

void creat_channels(int id){

  int oflago,oflagi;

  char file_name_out_node[]="output_0.txt";
  char file_name_in_node[]="input_0.txt";
  file_name_out_node[7] += id;
  file_name_in_node[6] += id;

  oflago = (O_TRUNC | O_CREAT | O_WRONLY);
  fdo = open(file_name_out_node, O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
  oflagi = (O_TRUNC | O_CREAT | O_RDONLY);
  fdi = open(file_name_in_node, O_RDONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
  if (fdo < 0 ) {printf("Error opening\n"); exit(1);}
  if (fdi < 0 ) {printf("Error opening\n"); exit(1);}

  return;
}

void send_hello(){

  char hello[]="hello 0\n";
  hello[6] += id_node;
  write(fdo, hello, 8); 

}

void process_inputs(){  //process read_buffer...

  char line_read[100];            //buffer to store the message line by line
  num_read_input = read(fdi, read_buffer, 1000);    //message is now in buffer
  char *ptr = read_buffer;
  int len = 0;

  while(ptr < (read_buffer + num_read_input)){ // + 1 or not??? num indicate chars read.//range is wrong!!!!????????
    memset(line_read,'\0',100);
    int idx = strchr(ptr, '\n')-ptr; //idx says where 1st '\n' is//wrong value of idx second loop//ptr wrong
    strncpy(line_read, ptr, idx+1);   
    ptr = ptr+idx+1; //ptr moving forward next line!
    len=strlen(line_read);
	//if(id_node==1)
		//printf("%s", line_read);
		
    if (line_read[0] == 'h'){   //hello message		
		int i = line_read[6]-'0';
		hello_from[i] = 15;
		
    }else if(line_read[0] == 'l'){  //linkstate message

      //1.store it in a LSA database for future use
		int num_parents = (len - 15)/2;
		int j;
		for(j = 0; j<10; j++){
        if (j == atoi(&line_read[10])){
          if (linkstate_database[j] == -1){
            linkstate_database[j] = (char*)malloc(33*sizeof(char));
            memcpy(linkstate_database[j], line_read, len); 
            write(fdo, line_read, len);
			linkstate_time[j]=30;
			clear_linkstate(j);
			int m;
			for (m=0; m<num_parents; m++){
				linkstate_mat[atoi(&line_read[15+2*m])][j] = 1;
			}
          }else{  //if the lsa is not new, write or not depend on timestamp

            int current = atoi(&read_buffer[12]);
            int prev = atoi(linkstate_database[j]+12);

            if (current > prev){
			  linkstate_time[j] = 30;
			  clear_linkstate(j);
              memcpy(linkstate_database[j], line_read, len);
              write(fdo, line_read, len);
			  int n;
				for (n=0; n<num_parents; n++){
				linkstate_mat[atoi(&line_read[15+2*n])][j] = 1;
				}
				
            }

          }
          
        }
      }
	  
    }else if(line_read[0] == 'd'){
  
		int s=atoi(&line_read[7]);
		int p=atoi(&line_read[5]);
	  //if it is the receiver, check root-id, if it needs the data, store it to fd_rec:
	  //if(is_receiver && (id_sender == s)&&(data_parent[s] == p)){
	  if(is_receiver && (id_sender==s) && (join_mat[id_node][id_sender]==p)){
	  write(fd_rec, line_read, len); 	  }
	  //if((data_child[s] != -1)&&(data_parent[s] == p)){//if it from the parent on the tree
	  int i=0;
	  for(i=0; i<10; i++)
	  {
		if((i!=id_node) && (join_mat[i][s]==p))
		{
		  //change sender id and forward
		  line_read[5] = (char)id_node + '0';		
			write(fdo, line_read, len);
			break;
		}
	  }

    }else if(line_read[0] == 'j'){
      //process join messages:
	  if(id_node == atoi(&line_read[7])){// if i am the sender who need to send the message:
		
		//no need to send the data here coz senders send data every 10s.
	  }else if(id_node == atoi(&line_read[9])){//if i am the parent on the shortest path tree:
		//get the sender id and send a join message!
		//and remember who i received this join from, i.e. the r in the join r s p x x x x ......
		temp_root_tree = atoi(&line_read[7]); //record sender on the tree
		temp_child_tree = atoi(&line_read[5]);	//record child on the tree
		id_sender = temp_root_tree;	//possible bugs?????
		join_mat[temp_child_tree][temp_root_tree]=send_join(temp_root_tree);
		join_time[temp_child_tree][temp_root_tree]=20;
		//data_parent[temp_root_tree]=send_join(temp_root_tree);
		//data_child[temp_root_tree]=temp_child_tree;
		
		int path_left_togo[10];
		int len_left_togo = shortest_path(id_node, id_sender, path_left_togo);
		temp_parent_tree = path_left_togo[1]; 
		//the current node will keep this message: temp_child_tree for what????

	  }else if(id_node == atoi(&line_read[11])){//if i am the first node on the path to the parent on the tree:
		//remove myself and forward this message to output file(fdo)
	  	char buffer[len];
		strncpy(buffer, line_read, 10);
		strncpy(buffer+10, line_read+12, len-12);
		write(fdo, buffer, len-2);
	  }else{
		//do nothing, ignore the message}
	  }
	  

    }else{

      //error message or did not read anything during this time

    }

    
  }
  //return;
 
}

void clear_linkstate(int node)
{
	int i=0;
	for(i=0; i<10; i++)
		linkstate_mat[i][node]=INF;
	linkstate_mat[node][node]=0;
}

void send_lsa(){// send lsa to output file


  char n_id[9]; // the list of n-IDs in lsa
  int hello_count = 0;
  int n = 0;
  int length_str = 0;

  memset(n_id,'\0',9);
	
	int j;
  for( j=0; j<10; j++){
    if (hello_from[j] > 0){

      hello_count++;
      n_id[n] = (char)j + '0'; 
      n++;

    }
  }

  length_str = (15+2*hello_count); //lsa string length including the '\n' 

  char lsa_temp_buffer[33];  //max length for a lsa is 33 including \n
  char* p = lsa_temp_buffer;
  char lsa_final_buffer[33];

  strncpy(lsa_temp_buffer, "linkstate i ts x x x x x x x x x\n", 33);
  int i;
  for ( i=0; i<9; i++){
    lsa_temp_buffer[15+2*i] = n_id[i];
  }
  int idx=strchr(p,'\0')-p;
  strncpy(lsa_final_buffer, p, idx);
  lsa_final_buffer[idx-1] = '\n';

  //add id_node to the message
  if (id_node == 0){
    lsa_final_buffer[10] = '0';
  }else if(id_node==1){
    lsa_final_buffer[10] = '1';
  }else if(id_node==2){
    lsa_final_buffer[10] = '2';
  }else if(id_node==3){
    lsa_final_buffer[10] = '3';
  }else if(id_node==3){
    lsa_final_buffer[10] = '3';
  }else if(id_node==4){
    lsa_final_buffer[10] = '4';
  }else if(id_node==5){
    lsa_final_buffer[10] = '5';
  }else if(id_node==6){
    lsa_final_buffer[10] = '6';
  }else if(id_node==7){
    lsa_final_buffer[10] = '7';
  }else if(id_node==8){
    lsa_final_buffer[10] = '8';
  }else if(id_node==9){
    lsa_final_buffer[10] = '9';
  }else{
  
  }

  int b = lsa_ts%10;
  int a = lsa_ts/10;

  lsa_final_buffer[12]='0'+a;
  lsa_final_buffer[13]='0'+b;

  write(fdo,lsa_final_buffer, idx);
  //write to database itself:
  if (linkstate_database[id_node] == -1){
    linkstate_database[id_node] = (char*)malloc(33*sizeof(char));
  }
  memcpy(linkstate_database[id_node], lsa_final_buffer, idx);
  int m; 
  int num_parents = (idx - 15)/2;
  clear_linkstate(id_node);
  linkstate_time[id_node]=30;
	for (m=0; m<num_parents; m++){
		linkstate_mat[atoi(&lsa_final_buffer[15+2*m])][id_node]=1;
	}
  
  lsa_ts++;

  return;

}

int shortest_path(int a, int b, int *path){

	int q[50];  //nodes to be processed
	int i = 0;
	int j = 0;
	int k;
	int dist[10];
	int prev[10];
	for(k=0; k<10; k++){
		prev[k]=-1;
		if(linkstate_mat[a][k] == 1)
		{
			dist[k]=1;
			prev[k]=a;
			q[j] = k; 
			j++;
		}
		else
		{	
			dist[k]=INF;
			prev[k]=-1;
		}
	}
	while(i != j){
		int temp = q[i]; //find next node after k
		for(k=0;k<10;k++){
			if((linkstate_mat[temp][k]==1) && (dist[temp]+1 < dist[k]))
			{
				q[j] = k;
				j++;
				prev[k]=temp;
				dist[k]=dist[temp]+1;
			}
		}
		i++;
	}
	
	int m = b;
	while(m != a){
		*(path + dist[m])=m;
		m=prev[m];
		if(m == -1)	return -1;
	}
	*path = a;
	return dist[b]+1; //return the number of hops of the shortest path!
}
	
int send_join(int source){
	
	int path_1[10];
	char join_temp_buffer[25];
	int len_path_1 = shortest_path(source, id_node, path_1); //return how many nodes on the path.
	if(len_path_1 == -1)	return -1;
	strncpy(join_temp_buffer, "join r s p              \n", 25);
	int length=10;
	
	int parent_to_s =  path_1[len_path_1-2];
	join_temp_buffer[5] = (char)id_node + '0';
	join_temp_buffer[7] = (char)source + '0';
	join_temp_buffer[9] = (char)(parent_to_s) + '0';
	
	int path_2[10];
	int len_path_2 = shortest_path(id_node, parent_to_s, path_2);
	if(len_path_2 > 2){
		int i;
		for(i=1; i<len_path_2-1; i++)
		{
			join_temp_buffer[length]=' ';
			join_temp_buffer[length+1]=(char)path_2[i]+'0';
			length+=2;
		}
	}
	char join_final_buffer[25];
	strncpy(join_final_buffer, join_temp_buffer, length);
	join_final_buffer[length] = '\n';
	//printf("node %d: %s", id_node, join_final_buffer);
	write(fdo,join_final_buffer, length+1);
	return parent_to_s;
}

void send_data(){
	//the message is already in the sender_message...
	int len = strlen(data_temp_buffer);
	write(fdo, data_temp_buffer, len); 
}


