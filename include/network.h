/**
 * Simulates a network for use in this programming assignment
 * Sends application traffic over udp connection and listens for
 * traffic over same udp connection
 */
class Network
{
  public:
    /**
     * Establishes a UDP connection with another host
     * and creates a protocol object based on the mode
     * @param mode 0 for process per protocol 1 for process per message
     */
    Network(int mode);

    /**
     * tears down udp connection and cleans up
     */
    ~Network();

    /**
     * invokes the proper method for sending a message from an application level
     * to the other host
     * @param id specifies the id of the protocol to be used
     * @msg contains the message to be sent down the stack
     */
    void send(int id, char* msg);

   /**
    * Listens for incoming messages from other host and
    * once received begins sending the message up the network protocol stack
    * @return char* the message to be read out by the application
    */
    char* receive();

  private:
   /**
    * establishes a UDP connection with another network and
    * ensures a connection was successfully established
    * @return bool true if good connection false otherwise
    */
    bool make_udp();
};