#include "game-server.hpp"


broadcast_server::broadcast_server(const WordList *_wl, const std::string _dbfile) {
   lobby = new Lobby::Inst(_wl, _dbfile);

   // Initialize Asio Transport
   m_server.init_asio();

   // Register handler callbacks
   m_server.set_open_handler(bind(&broadcast_server::on_open,this,::_1));
   m_server.set_close_handler(bind(&broadcast_server::on_close,this,::_1));
   m_server.set_message_handler(bind(&broadcast_server::on_message,this,::_1,::_2));
}

void
broadcast_server::run(uint16_t port) {
   // listen on specified port
   m_server.listen(port);

   // Start the server accept loop
   m_server.start_accept();

   // Start the ASIO io_service run loop
   try {
      m_server.run();
   } catch (const std::exception & e) {
      std::cout << e.what() << std::endl;
   }
}

void
broadcast_server::on_open(connection_hdl hdl) {
   {
      lock_guard<mutex> guard(m_action_lock);
      auto con = m_server.get_con_from_hdl(hdl);
      std::cout << "on_open: " << &hdl << " " << con->get_resource() << std::endl;
      m_actions.push(action(SUBSCRIBE,hdl,nullptr,con->get_resource()));
   }
   m_action_cond.notify_one();
}

void
broadcast_server::on_close(connection_hdl hdl) {
   {
      lock_guard<mutex> guard(m_action_lock);
      //std::cout << "on_close" << std::endl;
      m_actions.push(action(UNSUBSCRIBE,hdl));
   }
   m_action_cond.notify_one();
}

void
broadcast_server::on_message(connection_hdl hdl, server::message_ptr msg) {
   // queue message up for sending by processing thread
   {
      lock_guard<mutex> guard(m_action_lock);
      m_actions.push(action(MESSAGE,hdl,msg));
   }
   m_action_cond.notify_one();
}

void
broadcast_server::process_messages() {
   while(1) {
      unique_lock<mutex> lock(m_action_lock);

      while(m_actions.empty()) {
         m_action_cond.wait(lock);
      }

      action a = m_actions.front();
      m_actions.pop();

      lock.unlock();

      if (a.type == SUBSCRIBE) {
         lock_guard<mutex> guard(m_connection_lock);
         m_connections.insert(a.hdl);

         lobby->handle_appear(a.hdl, a.resource);
      } else if (a.type == UNSUBSCRIBE) {
         lock_guard<mutex> guard(m_connection_lock);
         m_connections.erase(a.hdl);

         lobby->handle_disappear(a.hdl);
      } else if (a.type == MESSAGE) {
         lock_guard<mutex> guard(m_connection_lock);

         auto handleResponses = lobby->process_msg(a.hdl, a.msg);
         for (const auto & hdlResponse : handleResponses) {
            m_server.send(hdlResponse.first, hdlResponse.second, a.msg->get_opcode());
         }

      } else {
         // undefined.
      }
   }
}
