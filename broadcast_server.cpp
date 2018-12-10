#include <iostream>
#include <set>

#include "game-server.hpp"

/*#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>*/

Game::Inst *gi;

int main(int argc, char *argv[]) {
   if (argc != 2) {
      std::cout << "Usage: game-server <portnumber>" << std::endl << std::endl;
      return -1;
   }

   try {
      broadcast_server server_instance;

      // Start a thread to run the processing loop
      thread t(bind(&broadcast_server::process_messages,&server_instance));

      std::cout << "Listening on port=" << atoi(argv[1]) << std::endl;

      // Run the asio loop with the main thread
      server_instance.run(atoi(argv[1]));

      t.join();

   } catch (websocketpp::exception const & e) {
      std::cout << e.what() << std::endl;
   }
}
