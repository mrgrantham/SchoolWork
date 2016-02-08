#include <iostream>
#include <cstdlib>
// #include <pthread.h>        // to use POSIX threads
#include <array>
#include <algorithm>
#include <vector>
#include <stack>
#include <unistd.h>
#include <sstream>
//#include <stdlib.h>       // rand and srand
#include <random>
#include <chrono>         // C++ time library for seed
#include <time.h>         // time function to see rand
#include <fstream>        // for logging to file csv

#include <iomanip>          // manipulat strings for file printout
#include <locale>           // to format number according to location

#include <ctime>        // Gives funciton for a string with date and time

#include <thread> // Seeing if the C++11 threads work better
#include <mutex>  // Mutex lock for C++11 threads

/* These may be values >=0 */
#define NUMBER_OF_CUSTOMERS 50 
#define NUMBER_OF_RESOURCES 3

#define NUMBER_OF_REQUESTS 10           // This should define the number of rendom requests that each customer thread makes
#define NUMBER_OF_RELEASES 10
#define CYCLES_OF_REQ_REL  10           // This is the parent for loop for the requests and releases


using namespace std;

class FileController { 
    
public:
    void read();
    void write(string aString);
    void write_comma();
    void write_end_line();
    void write_customer_line(int customer_id);
    void write_line(string title, int a_array[]);
    void closeFile();
    void setFilename(string filename);
    FileController(string filename);
    FileController();
    ~FileController();
    void increment(int customer_id, int hit_or_miss);
    
private:
    string filestring;
    ofstream ofile;
    int log_array[NUMBER_OF_CUSTOMERS][2];         // Track request that were granted or denied
    
};

// Simulated Resource for full implmentation of banker's algorithm scenario

class Resource  {
public:
    Resource(int rtid,int rcid, string description = "DOLLARS");
    ~Resource();
    friend ostream &operator<<(ostream &output, const Resource& aResource);

    
private:
    int resource_type_id;
    int resource_count_id;
    string resource_description;
    
};

class Banker {
public:

    // Need to implement functions to deal with resource allocation
    
    Banker();
    
    Banker(char *arguments[], int argCount, struct thread_data *td, FileController &alogger);

    bool safeState();
    
    int request_resources(int customerID, int request[]);
    int release_resources(int customerID, int request[]);
    
    void setMax(int customerID, int resourceType, int resourceQuantity);
    
    int getMax(int customerID, int resourceType); // May be useful if trying to go beyond the limit of current resources to test errors
    int getNeed(int customerID, int resourceType);
    int getAvailable(int resourceType);
    int getAllocated(int customerID, int resourceType);
    
    void printAllPrivateStructures();
    void print_available();
    void print_maximum();
    void print_allocation();
    void print_need();
    void print_resource_list();
    
private:
    
    void give_resource(int customerID, int resource_type, int resource_amount);
    void get_back_resource(int customerID, int resource_type, int resource_amount);

    void update_need();
    void print_array(int array[], int length);
    void print_2D_array(int array[][NUMBER_OF_RESOURCES],int length1, int length2);
    
    FileController *logger;
    vector< stack<Resource*> > resources;
    
    struct thread_data *customer_thread_data;
    
    /* The available amount of each resource */
    int available[NUMBER_OF_RESOURCES];
    
    /* The maximum demand for each customer */
    int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    
    /* The amount currently allocated to each customer */
    int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    
    /* The remaining need of each customer */
    int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    
};


struct thread_data {
    int customer_id;
    Banker *theBanker;
    vector< stack<Resource*> > resources;
    FileController *logger;
};




bool lessThanOrEqual(int a[], int b[], int length){
    
    for(int i = 0; i < length; i++){
        if(a[i]>b[i]){
            return false;
        }
    }
    return true;
}

void add(int a[],int b[], int length){
    for(int i = 0; i < length; i++){
        a[i] = a[i] + b[i];
    }
}

void subtract(int a[], int b[], int length){
    for(int i = 0; i < length; i++){
        a[i] = a[i] - b[i];
    }
}

mutex  customer_mutex;
unsigned seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);  

void *customer(void* td) {
    
  
    
    struct thread_data *customer_data;
    
    customer_mutex.lock();
    customer_data = (struct thread_data *)td;
    Banker *theBanker = customer_data->theBanker;
    customer_mutex.unlock();
    
    // pthread_mutex_t mutex;
    /* create a mutex lock */
    // pthread_mutex_init(&mutex,NULL);
   

    
    //pthread_mutex_lock(&mutex);
    customer_mutex.lock();
    
    // cout << "Customer [" << customer_data->customer_id << "]" << endl;
    // theBanker->printAllPrivateStructures();
    
    // pthread_mutex_unlock(&mutex);
    customer_mutex.unlock();

    
    int request[NUMBER_OF_RESOURCES];
    int request_error;
    int release_error;
    
    bool customer_finished = false;
    
    
    while(!customer_finished){ // This loop will continually request randomly based on need
        
        /*
        customer_mutex.lock();
        cout << endl<< "Customer [" << customer_data->customer_id << "]" << endl;
        theBanker->print_maximum();
        theBanker->print_available();
        theBanker->print_allocation();
        theBanker->print_need();
        
        theBanker->print_resource_list();

        customer_mutex.unlock();

         */
        
        // prepare random request
        for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
            customer_mutex.lock();
            uniform_int_distribution<int> random_distribution(0,(theBanker->getNeed(customer_data->customer_id,i)));
            request[i] = random_distribution(generator);
            customer_mutex.unlock();
        }    
                
        // pthread_mutex_lock(&mutex);
        customer_mutex.lock();

        /* // Commenting out request data now that algorithm appears to function
        cout << "Customer[" << customer_data->customer_id << "]: Request { ";
        
        for (int req = 0; req < NUMBER_OF_RESOURCES;req++){
            cout << request[req];
            if(req < NUMBER_OF_RESOURCES - 1)
                cout << ", ";
            else
                cout << " }" << endl;
        }
        */
        request_error = theBanker->request_resources(customer_data->customer_id, request);
        
        // pthread_mutex_unlock(&mutex);
        customer_mutex.unlock();

        
        // prepare random release
        // Only release resources if the process have reached max
        
        bool noNeed = true;
        
        for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
            customer_mutex.lock();
            if (!(theBanker->getAllocated(customer_data->customer_id,i) == theBanker->getMax(customer_data->customer_id,i))) {
                noNeed = false;
            }
            customer_mutex.unlock();
        }
        
        if(noNeed){
            // Process has all necessary resources to complete and can now release them.
            
            for(int i = 0;i < NUMBER_OF_RESOURCES; i++){
                customer_mutex.lock();
                request[i] = theBanker->getAllocated(customer_data->customer_id,i);
                customer_mutex.unlock();
            }
            
            cout << "request: ";
            for(int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                cout << request[i] << ", "; 
            }      
            cout << " } " << endl;
            
            // pthread_mutex_lock(&mutex);
            customer_mutex.lock();


            release_error = theBanker->release_resources(customer_data->customer_id, request);

            // pthread_mutex_unlock(&mutex);
            customer_finished = true;
            cout << "Customer [" << customer_data->customer_id << "] completed" << endl;

            cout << "Resource List After Thread Completion:" << endl;
            // theBanker->print_resource_list();
            
            customer_mutex.unlock();
            

        }
        

        
    }   

    customer_mutex.lock();

    customer_data->logger->write_customer_line(customer_data->customer_id);
    
    // theBanker->printAllPrivateStructures();
    customer_mutex.unlock();


}

int main(int argc, char *argv[]) {

    // Set timer
    clock_t start;
    double duration;
    start = clock();

    // pthread_t threads[NUMBER_OF_CUSTOMERS] {0};
    vector<thread> threads;
    
    struct thread_data td[NUMBER_OF_CUSTOMERS];
    int rc;
    
    
    if (argc < (NUMBER_OF_RESOURCES + 1)) {
        cerr << "Too few arguments" << endl;
        exit(1);
    }
    
    if (argc > (NUMBER_OF_RESOURCES + 1)) {
        cerr << "Too many arguments" << endl;
        exit(1);
    }    
    
    for(int i =0; i < argc; i++ ){
        cout << "Value: " << argv[i] << endl;
    }
    
    for(int i = 1; i < argc; i++) {

        for(int j = 0; argv[i][j] != '\0' ; j++ ){
            if(argv[i][j] < '0' || argv[i][j] > '9') {
                cerr << "Error: All parameters must be in numerical form" << endl;
                exit(1);
            }

        }
    }
    
    // Create date-time string for filename
    
    time_t theTime;
    struct tm * timeData;
    char buffer[80];
    
    time(&theTime);
    timeData = localtime(&theTime);
    

    
    FileController *logger = new FileController(string("log.csv"));
    
    strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeData);
    string stringTime(buffer);
    string titlename_string = "Banker Run: " + stringTime;
    logger->write_end_line(); 
    logger->write(titlename_string);
    logger->write_end_line();
    
    Banker *aBanker = new Banker(argv, argc, td, *logger);

    logger->write(string("Resources: { "));
    for(int i = 0; i < NUMBER_OF_RESOURCES;i++){
        logger->write(string(to_string(aBanker->getAvailable(i))));
        if(i < NUMBER_OF_RESOURCES - 1){
            logger->write("--");
        }
    }
    logger->write(string("}"));
    logger->write_end_line();
    
    // Set Max resources for each customer
    for(int customer_id = 0; customer_id < NUMBER_OF_CUSTOMERS; customer_id++){
        for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
            // Establish the max of each type of resource that this customer can have
            uniform_int_distribution<int> random_distribution(1,(aBanker->getAvailable(i)));
            customer_mutex.lock();
            aBanker->setMax(customer_id,i,random_distribution(generator));
            customer_mutex.unlock();
            
        }

        cout << "Max for customer [" << customer_id << "] : { ";

        for(int i = 0; i < NUMBER_OF_RESOURCES; i++){

            cout << aBanker->getMax(customer_id,i); 
            if(i < NUMBER_OF_RESOURCES - 1){
                cout << ", ";
            }
        }
        cout << "};" << endl;   
    }
    
    // Create customer threads
    
    for( int i = 0; i < NUMBER_OF_CUSTOMERS;i++){
        
        td[i].customer_id = i; 
        td[i].theBanker = aBanker;
        td[i].logger = logger;
        for(int j = 0; j < NUMBER_OF_RESOURCES;j++){
            td[i].resources.push_back(stack<Resource*>());
        }
        
        // rc = pthread_create(&threads[i], NULL, customer, (void *)&td[i]); // Pthread code
        threads.push_back(thread(customer,&td[i]));
        
        // cout << "done!" << endl;
        
        /* // Error catching for pthread
        if(rc){
            cerr << "Error: Thread could not be created" << endl;
        }
        */
    }
    
    /*
    for(int i = 0 ; i < NUMBER_OF_CUSTOMERS;i++){
        // pthread_join(threads[i],NULL);
        threads[i].join();
    }
     */
    
    //sleep(3000);
    for(auto& thread : threads){
        thread.join();
    }
    
    double clock_cycles = (double)( clock() - start );
    duration = clock_cycles / (double) CLOCKS_PER_SEC;
    cout << "Program took " << duration << " seconds." << endl;
    cout << "Program took " << clock_cycles << " clock cycles." << endl;

    
    stringstream duration_str;
    duration_str << "Duration(seconds) , " << duration << endl;
    duration_str << "Duration(clock_cycles) , " << clock_cycles << endl;
    logger->write(string(duration_str.str()));

    logger->closeFile();
    
    return 0;
}


Banker::Banker(){
    
}

Banker::Banker(char *arguments[], int argCount, struct thread_data *td, FileController &alogger){

    cout << "Initializing Banker..." << endl;
    
    customer_thread_data = td;
    
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
        available[i] = atoi(arguments[i+1]);
        //cout << "Value in available: " << available[i] << endl;
        resources.push_back(stack<Resource*>());
        for(int j = 0; j < available[i]; j++){
            Resource *r = new Resource(i,j);
            resources[i].push(r);
        }
    }
    
    // print_resource_list();
    
    
    for(int i = 0 ; i < NUMBER_OF_CUSTOMERS; i++){
        for(int j = 0; j < NUMBER_OF_RESOURCES; j++){
            maximum[i][j] = 0;
            allocation[i][j] = 0;
            need[i][j] = 0;

        }
        add(need[i],maximum[i],NUMBER_OF_RESOURCES);

    }

    logger = &alogger;
}


bool Banker::safeState(){
        
    array<int, NUMBER_OF_RESOURCES> work;
    copy(begin(available),end(available),begin(work));
    array<bool, NUMBER_OF_CUSTOMERS> finish = {false};
    
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){

        if(!(finish[i]) && lessThanOrEqual(need[i],work.data(),NUMBER_OF_RESOURCES)){
            
            add(work.data(), allocation[i], NUMBER_OF_RESOURCES);
            finish[i] = true;
            
        }      
        
    }
    
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
        if(!(finish[i])){
            return false;
        }
    }
    
  return true;
    
}

int Banker::request_resources(int customerID, int request[]){
    
    if(lessThanOrEqual(request,need[customerID],NUMBER_OF_RESOURCES)){
        if(lessThanOrEqual(request,available,NUMBER_OF_RESOURCES)){
            subtract(available,request,NUMBER_OF_RESOURCES);
            add(allocation[customerID],request,NUMBER_OF_RESOURCES);
            subtract(need[customerID],request,NUMBER_OF_RESOURCES); 
            
            if(safeState()){
                // Allow resource request
                cout << "Request granted for Customer[" << customerID << "]" <<  endl;
                // Execute thread with resources
                for(int i = 0;i< NUMBER_OF_RESOURCES; i++){
                    give_resource(customerID,i,request[i]);
                }
                // Track requests granted
                logger->increment(customerID,1);
                return 0;
            } else {
                // Resource Must Wait
                // Rollback Changes
                add(available, request,NUMBER_OF_RESOURCES);
                subtract(allocation[customerID],request,NUMBER_OF_RESOURCES);
                add(need[customerID],request,NUMBER_OF_RESOURCES);
                // Track requests denied
                logger->increment(customerID,0);
                return -1;
            }
            
            
        }
    } else {
        // Throw error here because the process cannot request more of a resource than it needs
        cerr << "Error: Process customer has requested more of a given resource than it needs" << endl;
        cerr << "Request    { " << request[0] << ", " << request[1] << ", " << request[2]<< " }" << endl;
        cerr << "Need       { " << need[customerID][0] << ", " << need[customerID][1] << ", " << need[customerID][2]<< " }" << endl;

        return -1;
    }
    
}

int Banker::release_resources(int customerID, int release[]){
    for(int i = 0; i < NUMBER_OF_RESOURCES;i++){
        get_back_resource(customerID,i,release[i]);
    }
        subtract(allocation[customerID],release,NUMBER_OF_RESOURCES);
        add(available,release,NUMBER_OF_RESOURCES);
        subtract(maximum[customerID],maximum[customerID], NUMBER_OF_RESOURCES);
        update_need();
}


void Banker::setMax(int customerID, int resourceType, int resourceQuantity){
    // cout << "Customer [" << customerID << "] Resource [" << resourceType << "] Quantity: " << resourceQuantity << endl;
    maximum[customerID][resourceType] = resourceQuantity;
    need[customerID][resourceType] = resourceQuantity - allocation[customerID][resourceType];
}
    
int Banker::getMax(int customerID, int resourceType) // May be useful if trying to go beyond the limit of current resources to test errors
{
    return maximum[customerID][resourceType];
}

int Banker::getNeed(int customerID, int resourceType)
{
    return need[customerID][resourceType];
}

int Banker::getAvailable(int resourceType)
{
    return available[resourceType]; 
}

int Banker::getAllocated(int customerID, int resourceType){
    return allocation[customerID][resourceType];
}

// This are print functions for troubleshooting

void Banker::printAllPrivateStructures(){

    print_available();
    print_maximum();
    print_allocation();
    print_need();
    
}

void Banker::print_available(){
    cout << "Available" << endl;
    cout << "_________" << endl;
    
    print_array(available,NUMBER_OF_RESOURCES);
    cout << endl << "_________" << endl << endl;
    
}

void Banker::print_maximum(){
    cout << "Maximum" << endl;
    cout << "_______" << endl;
    
    print_2D_array(maximum,NUMBER_OF_CUSTOMERS,NUMBER_OF_RESOURCES);
    cout << "_______" << endl << endl;
}

void Banker::print_allocation(){
    cout << "Allocation" << endl;
    cout << "__________" << endl;
    
    print_2D_array(allocation,NUMBER_OF_CUSTOMERS,NUMBER_OF_RESOURCES);
    cout << "__________" << endl << endl;
}

void Banker::print_need(){
    cout << "Need" << endl;
    cout << "____" << endl;
    
    print_2D_array(need,NUMBER_OF_CUSTOMERS,NUMBER_OF_RESOURCES);
    cout << "____" << endl << endl;
}

void Banker::print_resource_list(){
    stack<Resource*> temp_stack;
    Resource *aResource;

    for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
        int size = resources[i].size();
        for(int j = 0; j < size; j++){
            aResource = resources[i].top();
            resources[i].pop();
            cout << *aResource << "--------------" << endl;
            temp_stack.push(aResource);
            aResource = NULL;
        }
        
        size = temp_stack.size();
        for(int j = 0; j < size; j++){
            aResource = temp_stack.top();
            temp_stack.pop();
            resources[i].push(aResource);
            aResource=NULL;
        }

        
        
    }
    
}

// Private functions

void Banker::give_resource(int customerID, int resource_type, int resource_amount){

    for(int i = 0; i < resource_amount; i ++){
        customer_thread_data[customerID].resources[resource_type].push(resources[resource_type].top());
        resources[resource_type].pop();
    }
    
}

void Banker::get_back_resource(int customerID, int resource_type, int resource_amount){
    // Put the resource back
    for(int i = 0; i < resource_amount; i ++){
        resources[resource_type].push(customer_thread_data[customerID].resources[resource_type].top());
        customer_thread_data[customerID].resources[resource_type].pop();
    }
}


void Banker::update_need(){
    int  temp[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {0};
    for(int i = 0; i < NUMBER_OF_CUSTOMERS; i++ ){
        subtract(need[i],need[i],NUMBER_OF_RESOURCES);
        add(temp[i],maximum[i],NUMBER_OF_RESOURCES);
        subtract(temp[i],allocation[i],NUMBER_OF_RESOURCES);
        add(need[i],temp[i],NUMBER_OF_RESOURCES);
        
    }
}

void Banker::print_array(int array[], int length){
    
    for(int i = 0; i < length ; i++){
        cout << array[i];
        if(i < length -1){
            cout << ", ";
        }
    }
    
}

void Banker::print_2D_array(int array[][NUMBER_OF_RESOURCES],int length1, int length2){
    
    for (int i = 0; i < length1 ; i++){
        cout << "Array " << i << ": ";
            
        print_array(array[i],length2);
        
        cout << endl;
          
    }
    
}


Resource::Resource(int rtid, int rcid, string description){
    resource_type_id = rtid;
    resource_count_id = rcid;
    resource_description = description;
}

Resource::~Resource(){
    
}

ostream &operator<<(ostream &output, const Resource &aResource){
    output << "Resource_Cnt_ID: " << aResource.resource_count_id << endl;
    output << "Resource_Typ_ID: " << aResource.resource_type_id << endl;
    return output;
}

// This will allow data output to gather intel on operating of the algorithm

void FileController::read()
    {
        std::ifstream ifile(filestring.c_str());
        string fileData;
        
        if ( ifile.fail() )
        {
            std::cerr << "File opening for read operation failed." << std::endl;
            std::exit(1);
        } else {
            std::stringstream buffer;
            buffer <<  ifile.rdbuf();
            fileData = string(buffer.str());
            std::cout << fileData;
           ifile.close();
        }  
    }
    
    
    void FileController::write(string aString)
    {
       stringstream buffer;
       buffer << aString;
       ofile << buffer.str();
    }

    void FileController::write_comma(){
        stringstream buffer;
        buffer << ",";
        ofile << buffer.str();
    }
    
    void FileController::write_end_line(){
        stringstream buffer;
        buffer << endl;
        ofile << buffer.str();
    }
    
    void FileController::write_customer_line(int customer_id){
        stringstream buffer;
        buffer << "Customer[" << customer_id << "]";
        string customer_string= buffer.str();
        write_line(customer_string,log_array[customer_id]);
    }

    
    void FileController::write_line(string title, int a_array[]){
        
        write(title);
        write_comma();
        
        for(int i = 0; i < (sizeof(a_array)/sizeof(int)); i++){
            ostringstream convert;
            convert << a_array[i];
            
            write(convert.str());
            if(i<((sizeof(a_array)/sizeof(int))-1))
                write_comma();
           
        }
        write_end_line();
        
    }
    
    void FileController::setFilename(string filename)
    {
        filestring = filename;
    }
    
    FileController::FileController(string filename)
    {
        filestring = filename;
        ofile.open(filename.c_str(),std::ofstream::out | std::ofstream::app);
        
        if ( ofile.fail() )
        {
            cerr << "File opening for write operation failed." << endl;
            exit(1);
        } 

        
        
        for(int i = 0; i < NUMBER_OF_CUSTOMERS;i++){
            for(int j = 0; j < 2; j++){
                log_array[i][j] = 0;
            }
        }
        
    }
    
    void FileController::closeFile(){
        ofile.close();
    }
    
    FileController::FileController()
    {
        filestring = "";
        
        
    }
    
    FileController::~FileController(){
       
    }
    
    void FileController::increment(int customer_id, int hit_or_miss){
        cout << "Customer ID: " << customer_id << " Hit/Miss: " << hit_or_miss << endl; 
        log_array[customer_id][hit_or_miss] = log_array[customer_id][hit_or_miss] + 1;
    }
