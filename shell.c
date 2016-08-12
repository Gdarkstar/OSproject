
/*
 * shell.c
 */
#define true  1
#define false 0

void main()
{
    char buffer[20];
    char filename[6];
    char fileBuff[13312];
	char fileName1[7];
	char fileName2[7];
    char readbuf[512];
    int fileLen = 0;
	int sectors;
    char content[12800];
    char path[7];
    int i, j, k, index;
    int procID;

    enableInterrupts();

    while(1)
    {
        
        for(i=0; i<20; i++)
            buffer[i] = 0;

        
        for(i=0; i<6; i++)
            filename[i] = 0;

        
        for(i=0; i<512; i++)
            readbuf[i] = 0;

        for(i=0; i<12800; i++)
            content[i] = 0;

        for(i=0; i<15; i++)
            path[i] = 0;

        // call printString()
        interrupt(0x21, 0, "SHELL:", 0, 0);
        interrupt(0x21, 14, 0, 0, 0);
        interrupt(0x21, 0, "> ", 0, 0);
        // call readString()
        interrupt(0x21, 1, buffer, 0, 0);
        // handle dir
        if(buffer[0] == 'd' && buffer[1] == 'i' && buffer[2] == 'r')
        {
            interrupt(0x21, 0, "\n\r", 0, 0);
            // call directory()
            interrupt(0x21, 3, 0, 0, 0);
        }
        // handle type
        else if(buffer[0] == 't' && buffer[1] == 'y' && buffer[2] == 'p' && 
                buffer[3] == 'e')
        {
            for(i=0; i<6; i++)
                filename[i] = buffer[i+5];
            // call readFile()
            interrupt(0x21, 6, filename, content, 0);
            interrupt(0x21, 0, "\n\r", 0, 0);
            interrupt(0x21, 0, content, 0, 0);
        }
        // handle del
        else if(buffer[0] == 'd' && buffer[1] == 'e' && buffer[2] == 'l')
        {
            for(i=0; i<6; i++)
                filename[i] = buffer[i+4];
            interrupt(0x21, 0, "\n\r", 0, 0);
            // call deleteFile()
            interrupt(0x21, 4, filename, 0, 0);
        }
        // handle create
        else if(buffer[0] == 'c' && buffer[1] == 'r' && buffer[2] == 'e' && 
                buffer[3] == 'a' && buffer[4] == 't' && buffer[5] == 'e')
        {
            for(j=0; j<512; j++)
                content[j] = 0;
            for(i=0; i<6; i++)
                filename[i] = buffer[i+7];

            interrupt(0x21, 0, "\n\r", 0, 0);
            interrupt(0x21, 0, "Enter file content:", 0, 0);
            interrupt(0x21, 0, "\n\r", 0, 0);

            j = 0;
            while(1)
            {
                i = 0;
                interrupt(0x21, 1, readbuf, 0, 0);
                interrupt(0x21, 0, "\n\r", 0, 0);
                if(readbuf[0] == 0xd && readbuf[1] == 0xa && readbuf[2] == 0x0)
                    break;
                while(readbuf[i] != 0x0)
                {
                    content[i+j] = readbuf[i];
                    i++;
                }
                j = i + j;
                for(k=0; k<512; k++)
                    readbuf[k] = 0x0;
            }
            // call writeFile()
            interrupt(0x21, 8, filename, content, 0);
        }
        // handle execute
        else if(buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'e' && 
                buffer[3] == 'c' && buffer[4] == 'u' && buffer[5] == 't' && 
                buffer[6] == 'e')
        {
            for(i=0; i<6; i++)
            {
                // wipe CRLF
                if(buffer[8+i] == 0xa || buffer[8+i] == 0xd)
                    filename[i] = 0;
                else
                    filename[i] = buffer[8+i];
            }
            interrupt(0x21, 0, "\n\r", 0, 0);
            interrupt(0x21, 9, filename, 0, 0);
        }
        else if (buffer[0]=='c' && buffer[1]=='o' && buffer[2]=='p' && buffer[3]=='y'){
		/* Get the file names Split at space char */

		index = 5;
		i = 0;
		while(buffer[index] != 0x20 && buffer[index] != 0x0){
			fileName1[i] = buffer[index];
			index++;
			i++;
		}
		fileName1[6] = '\0';

		i = 0;
		index++;
		while(buffer[index] != 0x20 && buffer[index] != 0x0){
			fileName2[i] = buffer[index];
			index++;
			i++;
		}
		fileName2[6] = '\0';
		/* Have the file names. Read in filename1 */
		interrupt(0x21,6,fileName1,fileBuff,0);
		/* Figure out how many sectors we need */

		while(fileBuff[fileLen] != 0x0){
			fileLen++;
		}		
		sectors = div(fileLen,512);
		/* Write fileBuff (with fileName1 inside of it) to fileName2 */
                  interrupt(0x21, 0, "\n\r", 0, 0);
		interrupt(0x21,8,fileName2,fileBuff,sectors+1);
		
	}
        else if(buffer[0] == 'k' && buffer[1] == 'i' && buffer[2] == 'l' && 
                buffer[3] == 'l')
        {
            procID = buffer[5] - '0';
            interrupt(0x21, 0, "\n\r", 0, 0);
            interrupt(0x21, 10, procID, 0, 0);
        }
        else if(buffer[0] == 'c' && buffer[1] == 'd')
        {
            interrupt(0x21, 0, "\n\r", 0, 0);
            if(buffer[3] == '/')
            {
                for(i=0; i<7; ++i) {
                    if(buffer[3+i] == 0xa || buffer[3+i] == 0xd)
                        path[i] = 0;
                    else
                        path[i] = buffer[3+i];
                }
                interrupt(0x21, 13, path, 0, 0);
            }
            else if (buffer[3] == '.' && buffer[4] == '.')
            {
                interrupt(0x21, 16, 0, 0, 0);
            }
            else
            {
                path[0] = '/';
                for(i=0; i<6; ++i) {
                    if(buffer[3+i] == 0xa || buffer[3+i] == 0xd)
                        path[1+i] = 0;
                    else
                        path[1+i] = buffer[3+i];
                }
                interrupt(0x21, 13, path, 0, 0);
            }
        }
        else if(buffer[0] == 'm' && buffer[1] == 'k' && buffer[2] == 'd' && 
                buffer[3] == 'i' && buffer[4] == 'r')
        {
            //interrupt(0x21, 0, "\n\rmkdir called\n\r", 0, 0);
            interrupt(0x21, 0, "\n\r", 0, 0);
            if(buffer[6] == ' ' || buffer[6] == 0)
            {
                interrupt(0x21, 0, "insufficient parameter: please provide directory name.\n\r", 0, 0);
            }
            for(i=0; i<6; ++i)
            {
                if(buffer[6+i] == 0xa || buffer[6+i] == 0xd)
                    filename[i] = 0;
                else
                    filename[i] = buffer[6+i];
            }
            interrupt(0x21, 15, filename, 0, 0);
        }
        else if(buffer[0] == 'p' && buffer[1] == 'w' && buffer[2] == 'd')
        {
            interrupt(0x21, 0, "\n\r", 0, 0);
            interrupt(0x21, 14, 0, 0, 0);
            interrupt(0x21, 0, "\n\r", 0, 0);
        }
        else {
            interrupt(0x21, 0, "\n\r", 0, 0);
            interrupt(0x21, 0, "Bad Command!", 0, 0);
            interrupt(0x21, 0, "\n\r", 0, 0);
        }
    }
}
int div(int a, int b){
    int q = 0;
    while(q*b <=a){
        q = q+1;
    }
    return q-1;

}
