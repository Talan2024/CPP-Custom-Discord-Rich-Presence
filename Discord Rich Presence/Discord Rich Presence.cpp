#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <ctime>


//variables for rich presence elements
std::string client_id;
std::string state;
std::string details;
std::string large_text;
std::string large_image;
std::string small_text;
std::string small_image;
std::string party_id;
int people_in_party;
int party_size;

//get the number of bytes  written in WriteFile function
DWORD bytesWritten = 0;

//create a handle to the discord pipe
HANDLE hPipe = CreateFile(
    L"\\\\?\\pipe\\discord-ipc-0",
    GENERIC_READ | GENERIC_WRITE,
    0, NULL, OPEN_EXISTING, 0, NULL
);


//get inputs for discord rich presence elements
void get_input() {
    std::cout << "enter discord client id:";
    std::getline(std::cin, client_id);

    std::cout << "details:";
    std::getline(std::cin, details);



    std::cout << "enter state:";
    std::getline(std::cin, state);


    std::cout << "enter large image(can use url to gif ex.https://c.tenor.com/WMiHLnhVvdUAAAAd/tenor.gif):";
    std::getline(std::cin, large_image);

    std::cout << "enter large text:";
    std::getline(std::cin, large_text);

    std::cout << "enter small text:";
    std::getline(std::cin, small_text);

    std::cout << "enter small image:";
    std::getline(std::cin, small_image);

    std::cout << "enter party_id:";
    std::getline(std::cin, party_id);

    std::cout << "enter people_in_party:";
    std::cin >> people_in_party;
    std::cin.ignore();

    std::cout << "enter party_size:";
    std::cin >> party_size;
    std::cin.ignore();





}


int handshake() {
    //json to connect to discord application 
    std::string handshake = "{\"v\":1,\"client_id\":\"" + client_id + "\"}";
    //discord opcode for HANDSHAKE
    int handshakeOpcode = 0;
    // get length of handshake json
    int handshakeLength = handshake.size();
    //get total size of payload
    int total_handshake_size = 8 + handshakeLength;

    //create space in memory to create the payload in binary 
    char* buffer_handshake = new char[total_handshake_size];
    //add the opcode which is 4 bytes
    memcpy(buffer_handshake, &handshakeOpcode, 4);
    // go up 4 bytes in the buffer to add the length of the handshake which is 4 bytes
    memcpy(buffer_handshake + 4, &handshakeLength, 4);
    //go up 8 bytes in the buffer to add content of handshake then the size of it
    memcpy(buffer_handshake + 8, handshake.c_str(), handshakeLength);


    //checks connection to pipe is valid or not
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cout << "Error connecting to named pipe: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Successfully connected to named pipe." << std::endl;
    //send the payload into the pipe
    WriteFile(hPipe, buffer_handshake, total_handshake_size, &bytesWritten, NULL);
    return 0;
}

void identify() {

    //json to identify yopurself to discord  
    std::string identify = R"({
        "cmd": "IDENTIFY",
        "args": {
            "token": "",
            "capabilities": 1,
            "properties": {
                "os": "windows",
                "browser": "my_cool_app",
                "device": "my_cool_app"
            },
            "compress": false,
            "client_id": ")" + client_id + R"(",
            "client_version": 1
        },
        "evt": null,
        "nonce": "identify_1234"
    })";


    //opcode for FRAME/commands
    int OPCODE_IDENTIFY = 1;
    // get the size of json string
    int LENGTH_IDENTIFY = identify.size();
    //get total size of payload
    int total_identify_size = 8 + LENGTH_IDENTIFY;
    //create space in memory to create the payload in binary 
    char* buffer_identify = new char[total_identify_size];
    //add the opcode which is 4 bytes
    memcpy(buffer_identify, &OPCODE_IDENTIFY, 4);
    // go up 4 bytes in the buffer to add the length of the handshake which is 4 bytes
    memcpy(buffer_identify + 4, &LENGTH_IDENTIFY, 4);
    // go up 8 bytes to write the actual identify json payload into the buffer
    memcpy(buffer_identify + 8, identify.c_str(), LENGTH_IDENTIFY);

    //send the payload into the pipe
    WriteFile(hPipe, buffer_identify, total_identify_size, &bytesWritten, NULL);
    std::cout << "Sent IDENTIFY" << std::endl;
}


void set_activity() {
    std::string activity = R"({
    "cmd": "SET_ACTIVITY",
    "args": {
        "pid": )" + std::to_string(GetCurrentProcessId()) + R"(,
        "activity": {
            "state": ")" + state + R"(",
            "details": ")" + details + R"(",
            "timestamps": {
                "start": )" + std::to_string(time(NULL)) + R"(
            },
            "assets": {
                "large_image": ")" + large_image + R"(",
                "large_text": ")" + large_text + R"(",
                "small_image": ")" + small_image + R"(",
                "small_text": ")" + small_text + R"("
            },
            "party": {
                "id": ")" + party_id + R"(",
                "size": [)" + std::to_string(people_in_party) + R"(, )" + std::to_string(party_size) + R"(]
            }
        }
    },
    "nonce": "set_activity_nonce"
})";


    //opcode for FRAME/commands
    int OPCODE_ACTIVITY = 1;
    //get size of json activity string
    int LENGTH_ACTIVITY = activity.size();

    // total packet size 
    int total_activity_size = 8 + LENGTH_ACTIVITY;
    //create space in memory to create the payload in binary 
    char* buffer_activity = new char[total_activity_size];
    //add the opcode which is 4 bytes
    memcpy(buffer_activity, &OPCODE_ACTIVITY, 4);
    // go up 4 bytes in the buffer to add the length of the OPCODE_ACTIVITY which is 4 bytes
    memcpy(buffer_activity + 4, &LENGTH_ACTIVITY, 4);
    // go up 8 bytes to write the actual set_activity json payload into the buffer
    memcpy(buffer_activity + 8, activity.c_str(), LENGTH_ACTIVITY);


    //send the payload into the pipeline
    WriteFile(hPipe, buffer_activity, total_activity_size, &bytesWritten, NULL);
    std::cout << "Sent SET_ACTIVITY" << std::endl;
}

void Heartbeat_loop() {
    std::cout << "Starting heartbeat loop..." << std::endl;

    //json string for heartbeat
    std::string heartbeat = "{}";
    //opcode for FRAME/commands
    int OPCODE_HEARTBEAT = 1;
    //get size of json heartbeat string
    int LENGTH_HEARTBEAT = heartbeat.size();

    // total packet size 
    int total_heartbeat_size = 8 + LENGTH_HEARTBEAT;

    //create space in memory to create the payload in binary 
    char* buffer_heartbeat = new char[total_heartbeat_size];
    //add the opcode which is 4 bytes
    memcpy(buffer_heartbeat, &OPCODE_HEARTBEAT, 4);
    // go up 4 bytes in the buffer to add the length of the buffer_heartbeat which is 4 bytes
    memcpy(buffer_heartbeat + 4, &LENGTH_HEARTBEAT, 4);
    // go up 8 bytes to write the actual heartbeat json payload into the buffer
    memcpy(buffer_heartbeat + 8, heartbeat.c_str(), LENGTH_HEARTBEAT);

    //forever loop that pings discord every 15 seconds to let them know we are stil here
    while (true) {
        WriteFile(hPipe, buffer_heartbeat, total_heartbeat_size, &bytesWritten, NULL);
        std::cout << "Sent heartbeat" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}

bool wait_for_ready() {

    DWORD bytesRead;


    //create space in memory to create the payload in binary 
    char buffer[1024];
    //get size of payload
    int buffer_size = sizeof(buffer);

    while (true) {
        // try to read data from the pipe
        if (ReadFile(hPipe, buffer, buffer_size, &bytesRead, NULL)) {


            int opcode;
            int length;
            //get the opcode from the first 4 bytes of the buffer
            memcpy(&opcode, buffer + 4, 4);
            //get the length from the next 4 bytes of the buffer
            memcpy(&length, buffer + 4, 4);

            //extract the json payload starting at byte 8
            std::string payload(buffer + 8, length);


            //check if the payload contains the READY event
            if (payload.find("\"evt\":\"READY\"") != std::string::npos) {
                std::cout << "Got READY response.\n";
                return true;
            }
        }
        else {
            std::cout << "Failed to receive READY. Error: " << GetLastError() << std::endl;
            return false;
        }
    }
}




int main() {
    get_input();
    //check if the pipe connection succeeded
    if (handshake() == 1) {
        return 1;

    }



    identify();
    wait_for_ready();
    set_activity();
    Heartbeat_loop();
}