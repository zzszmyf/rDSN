# include <dsn/internal/message_parser.h>
# include <dsn/internal/logging.h>

# define __TITLE__ "message.parser"

namespace dsn {

    message_parser::message_parser(int buffer_block_size)
    {
        create_new_buffer(buffer_block_size);
    }

    void message_parser::create_new_buffer(int sz)
    {
        std::shared_ptr<char> buffer((char*)::malloc(sz));
        _read_buffer = utils::blob(buffer, sz);
        _read_buffer_occupied = 0;
    }

    void message_parser::mark_read(int read_length)
    {
        dassert(read_length + _read_buffer_occupied <= _read_buffer.length(), "");
        _read_buffer_occupied += read_length;
    }

    // before read
    void* message_parser::read_buffer_ptr(int read_next)
    {
        if (read_next + _read_buffer_occupied >  _read_buffer.length())
        {
            // remember currently read content
            auto rb = _read_buffer.range(0, _read_buffer_occupied);
            
            // switch to next
            create_new_buffer(max(read_next + _read_buffer_occupied, _buffer_block_size));

            // copy
            memcpy((void*)_read_buffer.data(), (const void*)rb.data(), rb.length());
            _read_buffer_occupied = rb.length();
            
            dassert (read_next + _read_buffer_occupied <= _read_buffer.length(), "");
        }

        return (void*)(_read_buffer.data() + _read_buffer_occupied);
    }

    int message_parser::read_buffer_capacity() const
    {
        return _read_buffer.length() - _read_buffer_occupied;
    }

    //-------------------- dsn message --------------------

    dsn_message_parser::dsn_message_parser(int buffer_block_size)
        : message_parser(buffer_block_size)
    {
    }

    message_ptr dsn_message_parser::on_read(int read_length, __out_param int& read_next)
    {
        mark_read(read_length);

        if (_read_buffer_occupied >= message_header::serialized_size())
        {            
            int msg_sz = message_header::serialized_size() +
                message_header::get_body_length((char*)_read_buffer.data());

            // msg done
            if (_read_buffer_occupied >= msg_sz)
            {
                auto msg_bb = _read_buffer.range(0, msg_sz);
                message_ptr msg = new message(msg_bb, true);

                _read_buffer = _read_buffer.range(msg_sz);
                _read_buffer_occupied -= msg_sz;
                read_next = message_header::serialized_size();
                return msg;
            }
            else
            {
                read_next = msg_sz - _read_buffer_occupied;
                return nullptr;
            }
        }

        else
        {
            read_next = message_header::serialized_size() - _read_buffer_occupied;
            return nullptr;
        }
    }
}