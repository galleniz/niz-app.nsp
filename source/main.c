#include <string.h>
#include <stdio.h>

#include <switch.h>
#include <malloc.h>
#include <math.h>

// Proyecto modificado de playnote xd
#define SAMPLERATE 48000
#define CHANNELCOUNT 2
#define FRAMERATE (1000 / 30)
#define SAMPLECOUNT (SAMPLERATE / FRAMERATE)
#define BYTESPERSAMPLE 2
#define DEBUG true

void fill_audio_buffer(void* audio_buffer, size_t offset, size_t size, int frequency) {
    if (audio_buffer == NULL) return;
    
    u32* dest = (u32*) audio_buffer;
    for (int i = 0; i < size; i++) {
        s16 sample = 0.3 * 0x7FFF * sin(frequency * (2 * M_PI) * (offset + i) / SAMPLERATE);
        dest[i] = (sample << 16) | (sample & 0xffff);
    }
}

#define TEXT_COUNT 15
#define MEGALOVANIA_LENGTH 39

const char* texts[TEXT_COUNT] = {
    "Es tan buena la aplicacion, que no necesitas nada mas que el boton ''A''",
    "Osea en plan holy shit, tremenda app tío, osea todo en plan omg holy shit",
    "Es mi primera app creada, no me juzguen porfavor...",
    "Vale pondre, Hello world, ahora es una app oficial!!!",
    "Te imaginas que alguien lo tenga en un Switch?, en ese caso, si eres tu ese alguien, Gracias :D",
    "Cualqueir ''error'' que pase reporten en https://github.com/MrNiz/my-first-app-nx/",
    "Nah al chile hace calor ahora mismo",
    "Y si hago un Chat-GPT?",
    "Mejor no, me da hueva asldkajflkasfakl, perdonen",
    "Para los fans de FNF (osea 3 weyes creo); ¿Creen que se pueda portear Haxe acá?, dudo mucho, pero con esfuerzo todo se puede, creo",
    "Tengo sueno aahhahshah, tratare cada fin de semana agregar mas textos",
    "Sabian que no puedo poner tildes porque sino: áéíóúñ",
    
    "Meh, si gustas aportar algo puedes hacerlo, aca abajo anda el repo si gustas",
    "Imaginate ser niz, tremenda decepción",
    "No hay mas texto disponible. xd, perdon espera actualizaciones en el repositorio de abajo.\n",
    "This text will no appear | este texto no deberia aparecer creo"
};

int main(int argc, char **argv)
{
        Result rc = 0;

    int notefreq[] = {
        220,
        440, 880, 1760, 3520, 7040,
        14080,
        7040, 3520, 1760, 880, 440
    };
    // Canción de megalovania por TobyFox creo nose eso recuerdo ayuda me esta pegando mi mamá
    int notes[] = {
    1174, 1174, 2349, 1760, 1661, 1567, 1396, 1174, 1396, 1567, 
    1046, 1046, 2349, 1760, 1661, 1567, 1396, 1174, 1396, 1567, 
    918, 918, 2349, 1760, 1661, 1567, 1396, 1174, 1396, 1567, 
    1046, 1046, 2349, 1760, 1661, 1567, 1396, 1174, 1396, 1567, 

}; 
    int tempo = 125; // Tempo en BPM de megalovania
    int currentNote = 0; // el index de la nota xd
    int note_duration = (60 * 1000000) / (tempo * 4); // Duración de una nota de negra en microsegundos
    bool megalovania = false;
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    AudioOutBuffer audout_buf;
    AudioOutBuffer *audout_released_buf;
    u32 data_size = (SAMPLECOUNT * CHANNELCOUNT * BYTESPERSAMPLE);
    u32 buffer_size = (data_size + 0xfff) & ~0xfff;
    u8* out_buf_data = memalign(0x1000, buffer_size);
    if (out_buf_data == NULL)
    {
        rc = MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);
        printf(":u\n");
    }
    if (R_SUCCEEDED(rc))
        memset(out_buf_data, 0, buffer_size);
    if (R_SUCCEEDED(rc))
        rc = audoutInitialize();
    if (R_SUCCEEDED(rc))
        rc = audoutStartAudioOut();

    bool play_tone = false;
    const char* default_text = "Esta es la mejor aplicacion de Homebrew de Switch (presiona A para pasar al siguiente texto).";

    int currentTextIndex = 0;
    int cur = 550;

    printf("%s\n", default_text);
    printf("\n\n%s<\n\n", texts[currentTextIndex]);
    printf("\nDesarollado con mucho amor y carino, lagrimas y sudor, esfuerzo y toda esa madre por: MrNiz | Repositorio GitHub oficial: https://github.com/MrNiz/my-first-app-nx/");
    
    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        if (megalovania)
        {
    
            usleep(note_duration); // Esperar el tiempo de una nota de negra
            currentNote = (currentNote + 1) % MEGALOVANIA_LENGTH;
            fill_audio_buffer(out_buf_data, 0, data_size, notes[currentNote]);
            play_tone = true;
                 printf("\x1b[2J"); // Clear the console
                printf("\n\n\n :)");
        }
        if (kDown & HidNpadButton_Plus) break; // Break in order to return to hbmenu

        if (kDown & HidNpadButton_X) 
        {
            megalovania = !megalovania;
        }

        if (kDown & HidNpadButton_B) // duh
        {
            cur += 10;
        }

        if (kDown & HidNpadButton_A)
        {
            fill_audio_buffer(out_buf_data, 0, data_size, cur);
            play_tone = true;
            currentTextIndex = (currentTextIndex + 1) % TEXT_COUNT;
            printf("\x1b[2J"); // Clear the console
            printf("%s\n", default_text);

            if (currentTextIndex < TEXT_COUNT)
                printf("\n\n%s\n\n", texts[currentTextIndex]);
            else
                printf("esto no deberia aparecer, si aparece reportalo porfa xd.");
	        printf("\n\n\nDesarollado con mucho amor y carino, lagrimas y sudor, esfuerzo y toda esa madre por: MrNiz | Repositorio GitHub oficial: https://github.com/MrNiz/my-first-app-nx/");
        }
        if (R_SUCCEEDED(rc) && play_tone)
        {
            // Prepare the audio data source buffer.
            audout_buf.next = NULL;
            audout_buf.buffer = out_buf_data;
            audout_buf.buffer_size = buffer_size;
            audout_buf.data_size = data_size;
            audout_buf.data_offset = 0;

            // Prepare pointer for the released buffer.
            audout_released_buf = NULL;

            // Play the buffer.
            rc = audoutPlayBuffer(&audout_buf, &audout_released_buf);
            play_tone = false;
        }

        consoleUpdate(NULL);
    }
    // Stop audio playback.
    rc = audoutStopAudioOut();
    printf("audoutStopAudioOut() returned 0x%x\n", rc);

    // Terminate the default audio output device.
    audoutExit();

    consoleExit(NULL);
    return 0;
}
