#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define pi 3.14

typedef struct node1
{
    char name[20];
    float top_speed, acc, max_load;
    struct node1 *next;
}Drone_t;

typedef struct node2
{
    char name[10];
    float weight, desti_lati, desti_long;
    int qty;
    struct node2 *next;
}Deliveries_t;


/*************************************************/
//Function Prototypes
void display_ll1(struct node1*);
void display_ll2(struct node2*);
Drone_t* read_drone_file(FILE*, Drone_t*);
Deliveries_t* read_deliveries_file(FILE*, Deliveries_t*);
int search_drone(Drone_t* , char [], float );
void update_deliveries(Deliveries_t* head);
float deg2rad(float );
float rad2deg(float );
float calc_distance(float , float , float , float );
void calc_time(float , float , float , float , float, int*, int* );
/*************************************************/



int main()
{
    FILE* fp=fopen("drones.dro","r");  //file pointer to drones.dro file
    Drone_t *head_drone=NULL;
    head_drone=read_drone_file(fp,head_drone);  //reads from drone.dro file and stores in a linked list
    fclose(fp);

    fp=fopen("deliveries.dro","r");   //file pointer to deliveries.dro file
    Deliveries_t *head_deliv=NULL;
    head_deliv=read_deliveries_file(fp,head_deliv);  //reads from deliveries.dro file and stores in a linked list
    fclose(fp);

    int ch, return_val_search, hr, min;
    float lati=0, longi=0, total_load;
    Deliveries_t *new_del;
    Deliveries_t *temp;
    char drone_name[10];
    FILE* fp_log;

    do
    {
        printf("\n1. Display available Drone\n");
        printf("2. Display Deliveries\n");
        printf("3. Set Warehouse Location\n");
        printf("4. New Delivery\n");
        printf("5. Dispatch\n");
        printf("6. Exit\n");
        scanf("%d",&ch);
        switch(ch)
        {
        case 1:
            display_ll1(head_drone);
            break;

        case 2:
            display_ll2(head_deliv);
            break;

        case 3:
            printf("Enter Co-ordinates from where the Drone be  dispatched,\nLatitude: ");
            scanf("%f",&lati);
            printf("Longitude: ");
            scanf("%f",&longi);
            printf("\nThe drone will be dispatched from warehouse located at %f  %f co-ordinates\n",lati,longi);
            break;

        case 4:
            new_del=(Deliveries_t*)malloc(sizeof(Deliveries_t));
            printf("Name: ");
            scanf("%s",new_del->name);
            printf("Quantity: ");
            scanf("%d",&new_del->qty);
            printf("Weight Per Item: ");
            scanf("%f",&new_del->weight);
            printf("Destination Latitude: ");
            scanf("%f",&new_del->desti_lati);
            printf("Destination Longitude: ");
            scanf("%f",&new_del->desti_long);
            new_del->next=NULL;

            if(head_deliv==NULL)
            {
                head_deliv=new_del;
            }
            else
            {
                temp=head_deliv;
                while(temp->next!=NULL)
                {
                    temp=temp->next;
                }
                temp->next=new_del;
            }

            display_ll2(head_deliv);
            break;

        case 5:
            if(lati==0 && longi==0)
                printf("\nWarehouse location not set! Set the warehouse location from the following menu...\n");
            else if(head_deliv!=NULL)
            {
                printf("Enter the name of Drone to dispatch:\n");
                scanf("%s",drone_name);
                total_load=head_deliv->qty * head_deliv->weight;
                return_val_search=search_drone(head_drone,drone_name,total_load);
                if(return_val_search==1)
                {
                    printf("\nDrone dispatching...\n");
                    printf("Saving Log in Dispatch_Log.txt...\n\n");
                    calc_time(head_drone->top_speed, lati, longi, head_deliv->desti_lati, head_deliv->desti_long, &hr, &min);
                    printf("Time taken by the drone to deliver is: %d hrs %d min\n",hr,min);

                    //update Dispatch_Log.txt
                    fp_log=fopen("Dispatch_Log.txt","a+");
                    fprintf(fp_log,"Name of Drone: %s\nDelivery Item: %s\nDelivery Quantity: %d\nWeight of Item delivered: %f\nDestination Latitude: %f\nDestination Longitude: %f\nTime Taken to deliver: %d hrs %d min\n\n",drone_name, head_deliv->name, head_deliv->qty, head_deliv->weight, head_deliv->desti_lati, head_deliv->desti_long, hr, min);
                    fclose(fp_log);

                    //removing the dispatched delivery from the linked list
                    temp=head_deliv;
                    head_deliv=head_deliv->next;
                    free(temp);
                }
                else if(return_val_search==-1)
                    printf("\nThe selected drone cannot carry the delivery as it exceed the load capacity.\n\n");
                else
                    printf("\nDrone NOT found!!\n\n");
            }
            else
                printf("\n!!There are NO more Deliveries to do. Relax!!\n");
            break;

        case 6:
            printf("\n1. Save and Exit\n");
            printf("2. Exit without Saving\n");
            scanf("%d",&ch);
            if(ch==1)
            {
                //writing deliveries.dro with updated deliveries
                update_deliveries(head_deliv);
                return 0;
            }
            else if(ch==2)
                return 0;
            break;

        default:
            printf("\nInvalid choice. Try again\n\n");
            break;
        }
    }while(1);

}



int search_drone(Drone_t* head, char name[], float load)
{
    int flag=0;
    while(head!=NULL)
    {
        if(strcmp(head->name,name)==0)
        {
            if(load <= head->max_load)
            {
                flag=1;
                break;
            }
            else
            {
                flag=-1;
                break;
            }
        }
        head=head->next;
    }
    if(flag)
    {
        if(flag==1)
            return 1;
        else
            return -1;
    }
    else
        return 0;
}

void update_deliveries(Deliveries_t* head)
{
    FILE* fp_deliv=fopen("deliveries.dro","w");

    while(head!=NULL)
    {
        fprintf(fp_deliv,"%s %d %f %f %f",head->name,head->qty,head->weight,head->desti_lati,head->desti_long);
        head=head->next;
        if(head!=NULL)
            fprintf(fp_deliv,"\n");
    }
    fclose(fp_deliv);
}

Drone_t* read_drone_file(FILE* fp, Drone_t* head1)
{
    Drone_t *new_drone;
    char buffer[100];
    do
    {
        new_drone=(Drone_t*)malloc(sizeof(Drone_t));

        fscanf(fp,"%s",new_drone->name);
        fscanf(fp,"%f",&new_drone->top_speed);
        fscanf(fp,"%f",&new_drone->acc);
        fscanf(fp,"%f",&new_drone->max_load);
        new_drone->next=head1;
        head1=new_drone;
    }while(!feof(fp));

return head1;
}

Deliveries_t* read_deliveries_file(FILE* fp, Deliveries_t* head2)
{
    Deliveries_t *new_del;
    Deliveries_t *temp;
    char buffer[100];
    do
    {
        new_del=(Deliveries_t*)malloc(sizeof(Deliveries_t));

        fscanf(fp,"%s",new_del->name);
        fscanf(fp,"%d",&new_del->qty);
        fscanf(fp,"%f",&new_del->weight);
        fscanf(fp,"%f",&new_del->desti_lati);
        fscanf(fp,"%f",&new_del->desti_long);
        new_del->next=NULL;
        if(head2==NULL)
        {
            head2=new_del;
            temp=new_del;
        }
        else
        {
            temp->next=new_del;
            temp=temp->next;
        }

    }while(!feof(fp));

return head2;
}

float deg2rad(float deg)
{
return (deg * pi / 180);
}

float rad2deg(float rad)
{
return (rad * 180 / pi);
}

float calc_distance(float lati1, float longi1, float lati2, float longi2)
{
    float theta, dist;
    theta = longi1 - longi2;
    dist = sin(deg2rad(lati1)) * sin(deg2rad(lati2)) + cos(deg2rad(lati1)) * cos(deg2rad(lati2)) * cos(deg2rad(theta));
    dist = acos(dist);
    dist = rad2deg(dist);
    dist = dist * 60 * 1.1515;
printf("\nDistance is %.0f miles\n",dist);
return (dist);
}

void calc_time(float speed, float lati1, float longi1, float lati2, float longi2, int* hr, int* min)
{
    float distance, time;
    int temp;

    distance=calc_distance(lati1, longi1, lati2, longi2);
    time=distance/speed;
    time=(time * 100);
    temp=time;
    *hr=temp/100;
    *min=temp%100;
    *min=*min*60/100;

}

void display_ll1(struct node1 *head1)
{
    if(head1==NULL)
        printf("\n!!There are NO drones available!!\n");
    else
    {
        printf("\n");
        while(head1!=NULL)
        {
            printf("%s ",head1->name);
            printf("%.3f ",head1->top_speed);
            printf("%.3f ",head1->acc);
            printf("%.3f ",head1->max_load);
            printf("\n");
            head1=head1->next;
        }
    }
}

void display_ll2(struct node2 *head2)
{
    if(head2==NULL)
        printf("\n!!There are NO Deliveries available!!\n");
    else
    {
        printf("\n");
        while(head2!=NULL)
        {
            printf("%s ",head2->name);
            printf("%d ",head2->qty);
            printf("%.3f ",head2->weight);
            printf("%.3f ",head2->desti_lati);
            printf("%.3f ",head2->desti_long);
            printf("\n");
            head2=head2->next;
        }
    }
}
