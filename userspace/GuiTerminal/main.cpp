#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"
#include "kernel/errs.h"
#include "asserts.h"
#include "LibWindowServer/IPC.h"
#include "LibGui/GuiManager.h"
#include "shared_ptr.h"
#include "LibGui/Widgets/TextView.h"

int stdin_fd = -1;
int stdout_fd = -1;

void create_terminal()
{
    char terminal[MAX_PATH];
    const int rc = std::create_terminal(terminal);
    ASSERT(rc == E_OK);
    Path terminal_path("/dev/pts/");
    terminal_path.add_part(terminal);

    Path stdin = terminal_path;
    stdin.add_part("in");

    Path stdout = terminal_path;
    stdout.add_part("out");

    stdin_fd = std::open(stdin.to_string().c_str());
    stdout_fd = std::open(stdout.to_string().c_str());

    kprintf("stdin_fd: %d\n", stdin_fd);
    kprintf("stdout_fd: %d\n", stdout_fd);

    ASSERT(stdin_fd == STDIN);
    ASSERT(stdout_fd == STDOUT);
}

void print_hello_text() {
    int fd = std::open("/init/hello.txt");
    ASSERT(fd>=0);
    int size = std::file_size(fd);
    ASSERT(size > 0);
    char* buff = new char[size+1];
    int rc = std::read(fd, buff, size);
    ASSERT(rc == size);
    buff[size] = 0;
    printf("%s\n", buff);
}

int main() {
    printf("gui!\n");
    std::sleep_ms(1000);

    create_terminal();

    print_hello_text();

    int pid = std::fork_and_exec("/bin/shell.app", "shell");
    ASSERT(pid > 0);

    Window window = GuiManager::the().create_window(600, 400);
    window.set_background_color(0x00000000);

    TextView* tv = new TextView(20,20,580,380);
    // tv->set_text("Hello World!");
    shared_ptr<Widget> text_view(tv);

    window.add_widget(text_view);

    for(;;)
    {


        GuiManager::the().draw(window);


        PendingInputBlocker::Reason reason = {};
        u32 ready_fd = 0;
        u32 fds[] = {static_cast<u32>(stdout_fd)};
        const int rc = std::block_until_pending(fds, 1, ready_fd, reason);
        ASSERT(rc == E_OK);
        
        if(reason == PendingInputBlocker::Reason::PendingMessage)
        {
            IOEvent io_event = GuiManager::the().get_io_event(); 
            if(io_event.type != IOEvent::Type::KeyEvent)
            {
                continue;
            }

            KeyEvent key_event = io_event.as_key_event();
            if(key_event.released || !key_event.to_ascii())
            {
                continue;
            }

            kprintf("gui: key event: %c\n", key_event.to_ascii());

            char c = key_event.to_ascii();
            const int rc = std::write(stdin_fd, &c, 1);
            ASSERT(rc == 1);
        }

        else if(reason == PendingInputBlocker::Reason::FdReady)
        {
            char buff[100];
            const int rc = std::read(stdout_fd, buff,100);
            ASSERT(rc != 0);
            buff[rc] = '\0';
            tv->set_text(tv->get_text() + String(buff));
        }

    }

    return 0;
}
