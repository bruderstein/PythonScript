'''

Sendkeys module moved back to ctypes.
For x64 systems, for example.

(c) 2009 Igor S. Mandrigin, Agnitum Ltd.

'''

from ctypes import windll

# from the internet
KEYEVENTF_KEYUP = 2
VK_NUMLOCK = 144
KEYEVENTF_EXTENDEDKEY = 1
KEYEVENTF_KEYUP = 2


def _key_down( vk ) :

    scan = windll.user32.MapVirtualKeyA( vk, 0 )
    windll.user32.keybd_event( vk, scan, 0, 0 )



def _key_up( vk ) :
    
    scan = windll.user32.MapVirtualKeyA( vk, 0 )
    windll.user32.keybd_event( vk, scan, KEYEVENTF_KEYUP, 0 )




def toggle_numlock( turn_on ) :
    '''
    toggle_numlock(int) ->  int

    Turns NUMLOCK on or off and returns whether
    it was originally on or off. 
    '''

    is_on = 0
    keys = [];

    is_on = windll.user32.GetKeyState( VK_NUMLOCK ) & 1

    if is_on != turn_on :	
        windll.user32.keybd_event(VK_NUMLOCK, 
                    69, 
                    KEYEVENTF_EXTENDEDKEY | 0, 
                    0);
        windll.user32.keybd_event(VK_NUMLOCK, 
                    69, 
                    KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 
                    0);

	

    return is_on


def char2keycode( char ) :
    '''
    char2keycode(char) -> int

    Converts character to virtual key code
    '''
    vk = windll.user32.VkKeyScanA( ord( char ) )
    return  vk



def key_down( key ) :
    '''
    key_down(int) -> None

    Generates a key pressed event.  Takes a
    virtual key code.
    '''
    vk = key
    # XXX exception if >= 256
    _key_down(  vk )



def key_up( key ) :
    '''
    key_up(int) -> None

    Generates a key released event.  Takes a
    virtual key code.
    '''

    vk = key
    # XXX exception if >= 256
    _key_up( vk )
