/*---------------------------------------------------------*/
/* ----------------   Proyecto Final ----------------------*/


/*------- Alumnos: Chavez Troncoso Hector Manuel -----------*/
/*-------          Moreno Osuna Isaac            -----------*/
/*-------          Rubio Carmona Alonso Rafael   -----------*/

//#define STB_IMAGE_IMPLEMENTATION
#include <glew.h>
#include <glfw3.h> 
#include <stb_image.h>
#include <windows.h>

#include "camera.h"
#include "Model.h"

// Other Libs
#include "SOIL2/SOIL2.h"
#include <irrKlang.h>


void resize(GLFWwindow* window, int width, int height);
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
// Window size
int SCR_WIDTH = 3800;
int SCR_HEIGHT = 7600;

GLFWmonitor *monitors;
GLuint VBO, VAO, EBO;

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 13.0f));
double	lastX = 0.0f,
		lastY = 0.0f;
bool firstMouse = true;

//Sound 
using namespace irrklang;
ISoundEngine *SoundEngine = createIrrKlangDevice();

//Timing
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(100.0f, 1.0f, -100.0f);
glm::vec3 lightDirection(-1.0f, -1.0f, -1.0f);
glm::vec3 lightDirection2(-1.0f, -2.0f, -11.0f);


void myData(void);
void display(Shader, Model, Model);
void getResolution(void);
void animate(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool);

//For Keyboard
float	movX = 0.0f,
		movY = 0.0f,
		movZ = -5.0f,
		rotX = 0.0f;

//Texture
unsigned int	t_nyan[20],
				t_skeleton[146],
				t_opening[1644],
				t_papelPicado,
				t_suelo,
				t_solid,
				t_vidriTrans,
				t_vidrPlas,
				t_vidrio,
				t_suelAlum,
				t_techo,
				t_barra;

//Animacion Gif

bool calavera = false;
bool huesos = true;
bool nyancat = false;
int framenyan = 0;
int frameSkeleton = 0;
float posiciones[4] = {-33.0f, -35.25f, -69.25f, -0.5f};

//Ghost 
bool fantasma = true;
float movG = 0.0f;
float pop = 0.0f;
float seno = 0.0f;

//Pumkim
float	rotDer = 0.0f,
		rotIzq = 0.0f;
bool calabaza = false;

//Keyframes Variables de dibujo para la animacion
float	posX = 0.0f, 
		posY = 0.0f, 
		posZ = 0.0f, 
		rotRodIzq = 0.0f,
		giroMonito = 0.0f;
float	movBrazo = 0.0f;

//Incrementos
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotInc = 0.0f,
		giroMonitoInc = 0.0f;
float	movBrazoInc = 0.0f;

float movLuzX = 0.0f;

float	luzRoja = 0.5f,
		luzVerde = 0.2f,
		luzAzul = 0.7f,
		r = 0.0f,
		g = 0.0f,
		b = 0.0f;

short estado = 0;

#define MAX_FRAMES 10
//cuadros intemedios
int i_max_steps = 15;

int i_curr_steps = 0;


typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;
	float movBrazo;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;
	KeyFrame[FrameIndex].movBrazo = movBrazo;


	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
	movBrazo = KeyFrame[0].movBrazo;

}

void interpolation(void)
{

	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	movBrazoInc = (KeyFrame[playIndex + 1].movBrazo - KeyFrame[playIndex].movBrazo) / i_max_steps;

}


unsigned int generateTextures(const char* filename, bool alfa)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		std::cout << filename << std::endl;
		return 100;
	}

	stbi_image_free(data);

}



//Funcion de resolucion 
void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;

	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;

}

void LoadTextures()
{
	t_techo = generateTextures("Texturas/white.jpg", 0);
	t_suelo = generateTextures("Texturas/suelo.jpg", 0);
	t_suelAlum = generateTextures("Texturas/sueloAlum.jpg", 0);
	t_solid = generateTextures("Texturas/paredBaja.png", 1);
	t_vidriTrans = generateTextures("Texturas/transpGlass.png", 1);
	t_barra = generateTextures("Texturas/Brushed_Metal.jpg", 0);
	t_vidrio = generateTextures("Texturas/glass.jpg", 0);
	t_vidrPlas = generateTextures("Texturas/vidrioplastico.jpg", 0);
	t_papelPicado = generateTextures("Texturas/ppDiaDeMuertos.png", 1);

	t_nyan[0] = generateTextures("TexturasJojo/nyancat/nyan00106.png", 0);
	t_nyan[1] = generateTextures("TexturasJojo/nyancat/nyan00111.png", 0);
	t_nyan[2] = generateTextures("TexturasJojo/nyancat/nyan00116.png", 0);
	t_nyan[3] = generateTextures("TexturasJojo/nyancat/nyan00121.png", 0);
	t_nyan[4] = generateTextures("TexturasJojo/nyancat/nyan00126.png", 0);
	t_nyan[5] = generateTextures("TexturasJojo/nyancat/nyan00131.png", 0);
	t_nyan[6] = generateTextures("TexturasJojo/nyancat/nyan00136.png", 0);
	t_nyan[7] = generateTextures("TexturasJojo/nyancat/nyan00141.png", 0);
	t_nyan[8] = generateTextures("TexturasJojo/nyancat/nyan00146.png", 0);
	t_nyan[9] = generateTextures("TexturasJojo/nyancat/nyan00151.png", 0);
	t_nyan[10] = generateTextures("TexturasJojo/nyancat/nyan00156.png", 0);
	t_nyan[11] = generateTextures("TexturasJojo/nyancat/nyan00161.png", 0);
	t_nyan[12] = generateTextures("TexturasJojo/nyancat/nyan00166.png", 0);
	t_nyan[13] = generateTextures("TexturasJojo/nyancat/nyan00171.png", 0);
	t_nyan[14] = generateTextures("TexturasJojo/nyancat/nyan00176.png", 0);
	t_nyan[15] = generateTextures("TexturasJojo/nyancat/nyan00181.png", 0);
	t_nyan[16] = generateTextures("TexturasJojo/nyancat/nyan00186.png", 0);
	t_nyan[17] = generateTextures("TexturasJojo/nyancat/nyan00191.png", 0);
	t_nyan[18] = generateTextures("TexturasJojo/nyancat/nyan00196.png", 0);
	t_nyan[19] = generateTextures("TexturasJojo/nyancat/nyan00201.png", 0);
	/*
	//Skeletons
	t_skeleton[0] = generateTextures("Texturas/skeleton/skeleton0.jpg", 0);
	t_skeleton[1] = generateTextures("Texturas/skeleton/skeleton1.jpg", 0);
	t_skeleton[2] = generateTextures("Texturas/skeleton/skeleton2.jpg", 0);
	t_skeleton[3] = generateTextures("Texturas/skeleton/skeleton3.jpg", 0);
	t_skeleton[4] = generateTextures("Texturas/skeleton/skeleton4.jpg", 0);
	t_skeleton[5] = generateTextures("Texturas/skeleton/skeleton5.jpg", 0);
	t_skeleton[6] = generateTextures("Texturas/skeleton/skeleton6.jpg", 0);
	t_skeleton[7] = generateTextures("Texturas/skeleton/skeleton7.jpg", 0);
	t_skeleton[8] = generateTextures("Texturas/skeleton/skeleton8.jpg", 0);
	t_skeleton[9] = generateTextures("Texturas/skeleton/skeleton9.jpg", 0);
	t_skeleton[10] = generateTextures("Texturas/skeleton/skeleton10.jpg", 0);
	t_skeleton[11] = generateTextures("Texturas/skeleton/skeleton11.jpg", 0);
	t_skeleton[12] = generateTextures("Texturas/skeleton/skeleton12.jpg", 0);
	t_skeleton[13] = generateTextures("Texturas/skeleton/skeleton13.jpg", 0);
	t_skeleton[14] = generateTextures("Texturas/skeleton/skeleton14.jpg", 0);
	t_skeleton[15] = generateTextures("Texturas/skeleton/skeleton15.jpg", 0);
	t_skeleton[16] = generateTextures("Texturas/skeleton/skeleton16.jpg", 0);
	t_skeleton[17] = generateTextures("Texturas/skeleton/skeleton17.jpg", 0);
	t_skeleton[18] = generateTextures("Texturas/skeleton/skeleton18.jpg", 0);
	t_skeleton[19] = generateTextures("Texturas/skeleton/skeleton19.jpg", 0);
	t_skeleton[20] = generateTextures("Texturas/skeleton/skeleton20.jpg", 0);
	t_skeleton[21] = generateTextures("Texturas/skeleton/skeleton21.jpg", 0);
	t_skeleton[22] = generateTextures("Texturas/skeleton/skeleton22.jpg", 0);
	t_skeleton[23] = generateTextures("Texturas/skeleton/skeleton23.jpg", 0);
	t_skeleton[24] = generateTextures("Texturas/skeleton/skeleton24.jpg", 0);
	t_skeleton[25] = generateTextures("Texturas/skeleton/skeleton25.jpg", 0);
	t_skeleton[26] = generateTextures("Texturas/skeleton/skeleton26.jpg", 0);
	t_skeleton[27] = generateTextures("Texturas/skeleton/skeleton27.jpg", 0);
	t_skeleton[28] = generateTextures("Texturas/skeleton/skeleton28.jpg", 0);
	t_skeleton[29] = generateTextures("Texturas/skeleton/skeleton29.jpg", 0);
	t_skeleton[30] = generateTextures("Texturas/skeleton/skeleton30.jpg", 0);
	t_skeleton[31] = generateTextures("Texturas/skeleton/skeleton31.jpg", 0);
	t_skeleton[32] = generateTextures("Texturas/skeleton/skeleton32.jpg", 0);
	t_skeleton[33] = generateTextures("Texturas/skeleton/skeleton33.jpg", 0);
	t_skeleton[34] = generateTextures("Texturas/skeleton/skeleton34.jpg", 0);
	t_skeleton[35] = generateTextures("Texturas/skeleton/skeleton35.jpg", 0);
	t_skeleton[36] = generateTextures("Texturas/skeleton/skeleton36.jpg", 0);
	t_skeleton[37] = generateTextures("Texturas/skeleton/skeleton37.jpg", 0);
	t_skeleton[38] = generateTextures("Texturas/skeleton/skeleton38.jpg", 0);
	t_skeleton[39] = generateTextures("Texturas/skeleton/skeleton39.jpg", 0);
	t_skeleton[40] = generateTextures("Texturas/skeleton/skeleton40.jpg", 0);
	t_skeleton[41] = generateTextures("Texturas/skeleton/skeleton41.jpg", 0);
	t_skeleton[42] = generateTextures("Texturas/skeleton/skeleton42.jpg", 0);
	t_skeleton[43] = generateTextures("Texturas/skeleton/skeleton43.jpg", 0);
	t_skeleton[44] = generateTextures("Texturas/skeleton/skeleton44.jpg", 0);
	t_skeleton[45] = generateTextures("Texturas/skeleton/skeleton45.jpg", 0);
	t_skeleton[46] = generateTextures("Texturas/skeleton/skeleton46.jpg", 0);
	t_skeleton[47] = generateTextures("Texturas/skeleton/skeleton47.jpg", 0);
	t_skeleton[48] = generateTextures("Texturas/skeleton/skeleton48.jpg", 0);
	t_skeleton[49] = generateTextures("Texturas/skeleton/skeleton49.jpg", 0);
	t_skeleton[50] = generateTextures("Texturas/skeleton/skeleton50.jpg", 0);
	t_skeleton[51] = generateTextures("Texturas/skeleton/skeleton51.jpg", 0);

	//Skeleton Part 2
	t_skeleton[52] = generateTextures("Texturas/spookey/spookey0.jpg", 0);
	t_skeleton[53] = generateTextures("Texturas/spookey/spookey1.jpg", 0);
	t_skeleton[54] = generateTextures("Texturas/spookey/spookey2.jpg", 0);
	t_skeleton[55] = generateTextures("Texturas/spookey/spookey3.jpg", 0);
	t_skeleton[56] = generateTextures("Texturas/spookey/spookey4.jpg", 0);
	t_skeleton[57] = generateTextures("Texturas/spookey/spookey5.jpg", 0);
	t_skeleton[58] = generateTextures("Texturas/spookey/spookey6.jpg", 0);
	t_skeleton[59] = generateTextures("Texturas/spookey/spookey7.jpg", 0);
	t_skeleton[60] = generateTextures("Texturas/spookey/spookey8.jpg", 0);
	t_skeleton[61] = generateTextures("Texturas/spookey/spookey9.jpg", 0);
	t_skeleton[62] = generateTextures("Texturas/spookey/spookey10.jpg", 0);
	t_skeleton[63] = generateTextures("Texturas/spookey/spookey11.jpg", 0);
	t_skeleton[64] = generateTextures("Texturas/spookey/spookey12.jpg", 0);
	t_skeleton[65] = generateTextures("Texturas/spookey/spookey13.jpg", 0);
	t_skeleton[66] = generateTextures("Texturas/spookey/spookey14.jpg", 0);
	t_skeleton[67] = generateTextures("Texturas/spookey/spookey15.jpg", 0);
	t_skeleton[68] = generateTextures("Texturas/spookey/spookey16.jpg", 0);
	t_skeleton[69] = generateTextures("Texturas/spookey/spookey17.jpg", 0);
	t_skeleton[70] = generateTextures("Texturas/spookey/spookey18.jpg", 0);
	t_skeleton[71] = generateTextures("Texturas/spookey/spookey19.jpg", 0);
	t_skeleton[72] = generateTextures("Texturas/spookey/spookey20.jpg", 0);
	t_skeleton[73] = generateTextures("Texturas/spookey/spookey21.jpg", 0);
	t_skeleton[74] = generateTextures("Texturas/spookey/spookey22.jpg", 0);
	t_skeleton[75] = generateTextures("Texturas/spookey/spookey23.jpg", 0);
	t_skeleton[76] = generateTextures("Texturas/spookey/spookey24.jpg", 0);
	t_skeleton[77] = generateTextures("Texturas/spookey/spookey25.jpg", 0);
	t_skeleton[78] = generateTextures("Texturas/spookey/spookey26.jpg", 0);
	t_skeleton[79] = generateTextures("Texturas/spookey/spookey27.jpg", 0);
	t_skeleton[80] = generateTextures("Texturas/spookey/spookey28.jpg", 0);
	t_skeleton[81] = generateTextures("Texturas/spookey/spookey29.jpg", 0);
	t_skeleton[82] = generateTextures("Texturas/spookey/spookey30.jpg", 0);
	t_skeleton[83] = generateTextures("Texturas/spookey/spookey31.jpg", 0);
	t_skeleton[84] = generateTextures("Texturas/spookey/spookey32.jpg", 0);
	t_skeleton[85] = generateTextures("Texturas/spookey/spookey33.jpg", 0);
	t_skeleton[86] = generateTextures("Texturas/spookey/spookey34.jpg", 0);
	t_skeleton[87] = generateTextures("Texturas/spookey/spookey35.jpg", 0);
	t_skeleton[88] = generateTextures("Texturas/spookey/spookey36.jpg", 0);
	t_skeleton[89] = generateTextures("Texturas/spookey/spookey37.jpg", 0);
	t_skeleton[90] = generateTextures("Texturas/spookey/spookey38.jpg", 0);
	t_skeleton[91] = generateTextures("Texturas/spookey/spookey39.jpg", 0);
	t_skeleton[92] = generateTextures("Texturas/spookey/spookey40.jpg", 0);
	t_skeleton[93] = generateTextures("Texturas/spookey/spookey41.jpg", 0);


	t_skeleton[94] = generateTextures("Texturas/spkskel/spkskel0.jpg", 0);
	t_skeleton[95] = generateTextures("Texturas/spkskel/spkskel1.jpg", 0);
	t_skeleton[96] = generateTextures("Texturas/spkskel/spkskel2.jpg", 0);
	t_skeleton[97] = generateTextures("Texturas/spkskel/spkskel3.jpg", 0);
	t_skeleton[98] = generateTextures("Texturas/spkskel/spkskel4.jpg", 0);
	t_skeleton[99] = generateTextures("Texturas/spkskel/spkskel5.jpg", 0);
	t_skeleton[100] = generateTextures("Texturas/spkskel/spkskel6.jpg", 0);
	t_skeleton[101] = generateTextures("Texturas/spkskel/spkskel7.jpg", 0);
	t_skeleton[102] = generateTextures("Texturas/spkskel/spkskel8.jpg", 0);
	t_skeleton[103] = generateTextures("Texturas/spkskel/spkskel9.jpg", 0);
	t_skeleton[104] = generateTextures("Texturas/spkskel/spkskel10.jpg", 0);
	t_skeleton[105] = generateTextures("Texturas/spkskel/spkskel11.jpg", 0);
	t_skeleton[106] = generateTextures("Texturas/spkskel/spkskel12.jpg", 0);
	t_skeleton[107] = generateTextures("Texturas/spkskel/spkskel13.jpg", 0);
	t_skeleton[108] = generateTextures("Texturas/spkskel/spkskel14.jpg", 0);
	t_skeleton[109] = generateTextures("Texturas/spkskel/spkskel15.jpg", 0);
	t_skeleton[110] = generateTextures("Texturas/spkskel/spkskel16.jpg", 0);
	t_skeleton[111] = generateTextures("Texturas/spkskel/spkskel17.jpg", 0);
	t_skeleton[112] = generateTextures("Texturas/spkskel/spkskel18.jpg", 0);
	t_skeleton[113] = generateTextures("Texturas/spkskel/spkskel19.jpg", 0);
	t_skeleton[114] = generateTextures("Texturas/spkskel/spkskel20.jpg", 0);
	t_skeleton[115] = generateTextures("Texturas/spkskel/spkskel21.jpg", 0);
	t_skeleton[116] = generateTextures("Texturas/spkskel/spkskel22.jpg", 0);
	t_skeleton[117] = generateTextures("Texturas/spkskel/spkskel23.jpg", 0);
	t_skeleton[118] = generateTextures("Texturas/spkskel/spkskel24.jpg", 0);
	t_skeleton[119] = generateTextures("Texturas/spkskel/spkskel25.jpg", 0);
	t_skeleton[120] = generateTextures("Texturas/spkskel/spkskel26.jpg", 0);
	t_skeleton[121] = generateTextures("Texturas/spkskel/spkskel27.jpg", 0);
	t_skeleton[122] = generateTextures("Texturas/spkskel/spkskel28.jpg", 0);
	t_skeleton[123] = generateTextures("Texturas/spkskel/spkskel29.jpg", 0);
	t_skeleton[124] = generateTextures("Texturas/spkskel/spkskel30.jpg", 0);
	t_skeleton[125] = generateTextures("Texturas/spkskel/spkskel31.jpg", 0);
	t_skeleton[126] = generateTextures("Texturas/spkskel/spkskel32.jpg", 0);
	t_skeleton[127] = generateTextures("Texturas/spkskel/spkskel33.jpg", 0);
	t_skeleton[128] = generateTextures("Texturas/spkskel/spkskel34.jpg", 0);
	t_skeleton[129] = generateTextures("Texturas/spkskel/spkskel35.jpg", 0);
	t_skeleton[130] = generateTextures("Texturas/spkskel/spkskel36.jpg", 0);
	t_skeleton[131] = generateTextures("Texturas/spkskel/spkskel37.jpg", 0);
	t_skeleton[132] = generateTextures("Texturas/spkskel/spkskel38.jpg", 0);
	t_skeleton[133] = generateTextures("Texturas/spkskel/spkskel39.jpg", 0);
	t_skeleton[134] = generateTextures("Texturas/spkskel/spkskel40.jpg", 0);
	t_skeleton[135] = generateTextures("Texturas/spkskel/spkskel41.jpg", 0);
	t_skeleton[136] = generateTextures("Texturas/spkskel/spkskel42.jpg", 0);
	t_skeleton[137] = generateTextures("Texturas/spkskel/spkskel43.jpg", 0);
	t_skeleton[138] = generateTextures("Texturas/spkskel/spkskel44.jpg", 0);
	t_skeleton[139] = generateTextures("Texturas/spkskel/spkskel45.jpg", 0);
	t_skeleton[140] = generateTextures("Texturas/spkskel/spkskel46.jpg", 0);
	t_skeleton[141] = generateTextures("Texturas/spkskel/spkskel47.jpg", 0);
	t_skeleton[142] = generateTextures("Texturas/spkskel/spkskel48.jpg", 0);
	t_skeleton[143] = generateTextures("Texturas/spkskel/spkskel49.jpg", 0);
	t_skeleton[144] = generateTextures("Texturas/spkskel/spkskel50.jpg", 0);
	t_skeleton[145] = generateTextures("Texturas/spkskel/spkskel51.jpg", 0); 
	*/
	int p = 0;
	int cond = 0;
	string ruta = "TexturasJojo/jojoOP/scene00001.png";
	//const char *rutac;
	char *rutac = new char[ruta.length() + 1];
	const char *consts[1645];
	//strcpy_s(rutac, ruta.c_str());
	//ruta.copy(rutac, ruta.length() + 1);
	//rutac[ruta.copy(rutac, sizeof(rutac) - 1)] = '\0';
	//printf("\n%s\n", rutac);
	/*
	int indice = 0;
	for (int l = 0; l < 2; l++)
	{
		for (int k = 0; k < 10; k++)
		{
			for (int j = 0; j < 10; j++)
			{
				for (int i = 0; i < 10; i++)
				{
					if (i > 0)
						ruta[29] = ruta[29] + 1;
					//rutac[ruta.copy(rutac, sizeof(rutac) - 1)] = '\0';
					strcpy(rutac,ruta.c_str());
					consts[indice] = rutac;
					t_opening[indice] = generateTextures(consts[indice], 0);
					indice++;
					//printf("\n%s\n",rutac);
					//cout << "\n" << ruta << "\n";
					if (j == 0 && i == 0)
						i++;
					if (l == 1 && k == 6 && j == 4 && i == 5)
					{
						cond = 1;
						break;
					}
				}
				if (cond == 1)
					break;
				ruta[29] = '0';
				ruta[28] = ruta[28] + 1;
				if (k == 0 && j == 0)
					j++;
			}
			if (cond == 1)
				break;
			ruta[28] = '0';
			ruta[27] = ruta[27] + 1;
		
		}
		if (cond == 1)
			break;
		ruta[27] = '0';
		ruta[26] = ruta[26] + 1;
	}
	//for (int i = 0; i < 1644; i++)
	//{
	//	t_opening[i] = generateTextures("Texturas/spkskel/spkskel51.jpg", 0);
	//}
	*/

}

void myData()
{	
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  50.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  25.0f,  50.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  25.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,

	};
	unsigned int indices[] = {

		// Front
		0, 1, 3, // first triangle
		1, 2, 3,  // second triangle

		//Back
		4, 6, 7, // first triangle
		4, 5, 6,  // second triangle

		//Left
		8, 9, 10, // first triangle
		8, 10, 11,  // second triangle

		//Right
		12, 13, 14, // first triangle
		12, 14, 15,  // second triangle

		//Up
		16, 17, 18, // first triangle
		16, 18, 19,  // second triangle

		//Down
		20, 21, 22, // first triangle
		20, 22, 23,  // second triangle
	};
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

}

void animate(void)
{

	r += 0.1;
	g += 0.2;
	b += 0.3;

	if (luzRoja <= 1.0f) {
		luzRoja += 0.003f;
	}
	else {
		luzRoja = 0.0f;
	}
	if (luzVerde <= 1.0)
	{
		luzVerde += 0.007f;
	}
	else {
		luzVerde = 0.0f;
	}
	if (luzAzul <= 1.0)
	{
		luzAzul += 0.005f;
	}
	else {
		luzAzul = 0.0f;
	}
	if (calabaza) {
		switch (estado)
		{
		case 1:
			if (rotDer >= 36)
				estado = 2;
			else
				rotDer++;
			break;

		case 0:
			if (rotIzq >= 36)
				estado = 1;
			else
				rotIzq++;
			break;

		case 3:
			if (rotDer <= 0)
				estado = 0;
			else
				rotDer--;
			break;

		case 2:
			if (rotIzq <= 0)
				estado = 3;
			else
				rotIzq--;
			break;
		}
	}


	if (calavera) {

		if (frameSkeleton < 1644) {
			Sleep(0);
			frameSkeleton++;
		}
		else 
		{
			frameSkeleton = 0;
		}	
	}

	if (nyancat) {
		if (framenyan < 19) {
			Sleep(20);
			framenyan++;
		}
		else
		{
			framenyan = 0;
		}
	}


	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotInc;
			giroMonito += giroMonitoInc;
			movBrazo += movBrazoInc;

			i_curr_steps++;

		}
	}

	if (movG < 360) {
		movG += 10.0f;
	}
	else {
		movG = 0.0f;
	}
	if (pop < 360) {
		pop += 10.0f;
	}
	else {
		pop = 0.0f;
	}
	seno = sin(glm::radians(pop));
}

void display(Shader shader, Model mesa, Model monitor, Model pc, Model keyboard, Model mouse, Model board, 
	         Model projector, Model fire, Model ghost, Model pumpkin, Model jotaro, Model garrafon)
{
	shader.use();


	shader.setVec3("viewPos", camera.Position);

	
	shader.setVec3("dirLight.direction", lightDirection);
	shader.setVec3("dirLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	//shader.setVec3("pointLight.position", lightPosition);
	shader.setVec3("pointLight[0].position", glm::vec3(0, 10, -35));
	shader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 1.0f));
	shader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 1.0f));
	shader.setFloat("pointLight[0].constant", 1.0f);
	shader.setFloat("pointLight[0].linear", 0.009f);
	shader.setFloat("pointLight[0].quadratic", 0.032f);

	shader.setVec3("pointLight[1].position", glm::vec3(sin(r*2), sin(g*5), sin(b*7)));
	shader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 1.0f, 0.0f));
	shader.setVec3("pointLight[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("pointLight[1].constant", 1.0f);
	shader.setFloat("pointLight[1].linear", 0.009f);
	shader.setFloat("pointLight[1].quadratic", 0.032f);

	shader.setVec3("pointLight[2].position", glm::vec3(sin(r*9), sin(g*7), sin(b*4)));
	shader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[2].diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("pointLight[2].constant", 1.0f);
	shader.setFloat("pointLight[2].linear", 0.009f);
	shader.setFloat("pointLight[2].quadratic", 0.032f);

	shader.setVec3("pointLight[3].position", glm::vec3(luzRoja, luzVerde + 10.0f, luzAzul - 39.0f));
	shader.setVec3("pointLight[3].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[3].diffuse", glm::vec3(luzRoja, luzVerde, luzAzul));
	shader.setVec3("pointLight[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("pointLight[3].constant", 1.0f);
	shader.setFloat("pointLight[3].linear", 0.009f);
	shader.setFloat("pointLight[3].quadratic", 0.032f);
	
	//shader.setVec3("spotLight.position", glm::vec3(-1.45f, 25.0f, -30.0f));
	shader.setVec3("spotLight.position", glm::vec3(-1.450f, 23.0f, -40.0f));
	shader.setVec3("spotLight.direction", lightDirection2);
	shader.setFloat("spotLight.cutOff", cos(glm::radians(13.0f)));
	shader.setFloat("spotLight.outerCutOff", cos(glm::radians(30.0f)));
	shader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.0009f);
	shader.setFloat("spotLight.quadratic", 0.0032f);

	shader.setFloat("material_shininess", 32.0f);

	// create transformations and Projection
	glm::mat4 temp01 = glm::mat4(1.0f);
	glm::mat4 tmp2 = glm::mat4(1.0f);
	glm::mat4 tmp3 = glm::mat4(1.0f);
	glm::mat4 tmp = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 view = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection

	//Use "projection" to include Camera
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glBindVertexArray(VAO);

	//PARED PIZARRON

	model = glm::translate(model, glm::vec3(0.0f, 0.25f, -70.25f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 8.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 15.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 2.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(34.75f, 4.5f, -70.25f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 8.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 8.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 12.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 15.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 15.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 9.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//Ventanales 

	model = glm::translate(glm::mat4(1.0f), glm::vec3(29.75f, 4.5f, -70.25f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 8.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_solid);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 8.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_solid);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 12.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 15.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidrPlas);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 15.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_vidrPlas);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 9.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 2.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidriTrans);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_papelPicado);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 2.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_vidriTrans);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
		shader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_papelPicado);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	// Pared EXTINTOR

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.25f, 0.25f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 8.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 15.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 2.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(34.75f, 4.5f, 0.25f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 8.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 8.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 12.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 15.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 15.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 9.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//Ventanales 

	model = glm::translate(glm::mat4(1.0f), glm::vec3(29.75f, 4.5f, 0.25f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 8.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_solid);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 8.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_solid);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 12.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 15.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidrPlas);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 15.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_vidrPlas);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	}

	model = glm::translate(temp01, glm::vec3(0.0f, 9.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 2.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidriTrans);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_papelPicado);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 2.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_vidriTrans);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		shader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_papelPicado);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	//Pared Puerta

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.25f, 0.25f, -35.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 8.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 15.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 2.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.5f, 0.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(25.0f, -22.25f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 8.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 5; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 8.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 12.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 15.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	for (int i = 0; i < 5; i++)
	{
		model = glm::translate(temp01, glm::vec3(10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 15.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(0.0f, 9.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 5; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 2.0f, 0.5f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_barra);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//Ventanales 

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.25f, 4.5f, -65.125f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.75f, 8.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_solid);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(-10.125f, 0.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 8.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_solid);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 4; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 8.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_solid);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(-10.125f, 0.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.75f, 8.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_solid);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//2DO NIVEL

	model = glm::translate(temp01, glm::vec3(0.0f, 12.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.75f, 15.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidrio);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(10.125f, 0.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 15.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidrio);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 4; i++)
	{
		model = glm::translate(temp01, glm::vec3(10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 15.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_vidrio);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(10.125f, 0.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.75f, 15.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidrio);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	//3ER NIVEL 

	model = glm::translate(temp01, glm::vec3(0.0f, 9.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.75f, 2.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidriTrans);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_papelPicado);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(-10.125f, 0.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.5f, 2.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidriTrans);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_papelPicado);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 4; i++)
	{
		model = glm::translate(temp01, glm::vec3(-10.0f, 0.0f, 0.0f));
		temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 2.0f, 0.1f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_vidriTrans);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
		shader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_papelPicado);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(temp01, glm::vec3(-10.125f, 0.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(9.75f, 2.0f, 0.1f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_vidriTrans);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 3.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_papelPicado);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.25f, 13.5f, -70.25f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 27.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-35.25f, 13.5f, 0.25f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 27.0f, 0.5f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//Suelo

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-21.5f, -0.05f, -35.0f));
	model = glm::scale(model, glm::vec3(27.0f, 0.1f, 70.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_suelo);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(17.5f, -0.05f, -35.0f));
	model = glm::scale(model, glm::vec3(45.0f, 0.1f, 70.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_suelo);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.5f, -0.05f, -35.0f));
	model = glm::scale(model, glm::vec3(3.0f, 0.1f, 70.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_suelAlum);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//
	//Pared Estacionamiento

	model = glm::translate(glm::mat4(1.0f), glm::vec3(37.5f, 13.5f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 27.0f, 2.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 0.0f, -45.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 27.0f, 2.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 0.0f, -25.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 27.0f, 2.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, -3.0f, 12.5f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	temp01 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 23.0f, 1.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(temp01, glm::vec3(0.0f, 35.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 43.0f, 1.5f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	model = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 5.0f, -35.0f));
	model = glm::scale(model, glm::vec3(1.0f, 10.0f, 70.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_barra);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 15.0f, -35.0f));
	model = glm::scale(model, glm::vec3(1.0f, 10.0f, 70.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_vidrio);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 27.0f, -35.0f));
	model = glm::scale(model, glm::vec3(70.0f, 0.1f, 70.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_techo);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	// Gif

	if (nyancat) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(posiciones[0], 15.0f, -30.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(10.0f, 15.5f, 0.1f));
		shader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_nyan[framenyan]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		/*
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 17.0f, posiciones[2]));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(18.0f, 13.0f, 0.1f));
		shader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, t_opening[frameSkeleton]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	    */
	}

	//Elemntos del salon
	tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(35.0f, 5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
	shader.setMat4("model", model);
	fire.Draw(shader);

	model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	tmp = model = glm::translate(model, glm::vec3(-5.0f, 69.5f, 10.0f));
	model = glm::scale(model, glm::vec3(0.16f, 0.16f, 0.16f));
	shader.setMat4("model", model);
	board.Draw(shader);

	float	x = -47.0f,
			y = 1.9f,
			z = 25.0f;
	for (int i = 0; i < 3; i++)
	{
		model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tmp = model = glm::translate(model, glm::vec3(x, y, z));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shader.setMat4("model", model);
		mesa.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 0.1f, 1 / 0.1f, 1 / 0.1f));

		tmp = model = glm::translate(model, glm::vec3(-2.0f, 3.78f, 2.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		shader.setMat4("model", model);
		monitor.Draw(shader);

		tmp = model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		monitor.Draw(shader);

		model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		monitor.Draw(shader);


		tmp = model = glm::scale(model, glm::vec3(1 / 0.7f, 1 / 0.7f, 1 / 0.7f));


		model = glm::translate(model, glm::vec3(-9.5f, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		shader.setMat4("model", model);
		pc.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 0.01f, 1 / 0.01f, 1 / 0.01f));

		model = glm::translate(tmp, glm::vec3(-5.5f, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		shader.setMat4("model", model);
		pc.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 0.01f, 1 / 0.01f, 1 / 0.01f));

		model = glm::translate(tmp, glm::vec3(-1.5f, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		shader.setMat4("model", model);
		pc.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 0.01f, 1 / 0.01f, 1 / 0.01f));

		model = glm::translate(tmp, glm::vec3(-0.9f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		shader.setMat4("model", model);
		keyboard.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-5.0f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		shader.setMat4("model", model);
		keyboard.Draw(shader);


		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));
		model = glm::translate(tmp, glm::vec3(-9.0f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		shader.setMat4("model", model);
		keyboard.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(0.4f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		shader.setMat4("model", model);
		mouse.Draw(shader);		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-3.8f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		shader.setMat4("model", model);
		mouse.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-7.8f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		shader.setMat4("model", model);
		mouse.Draw(shader);

		x += 15;

		//Sillas
		//Nothing here.

	}

	x = -47.0f;
	y = 3.0f;
	z = -10.0f;

	for (int i = 0; i < 3; i++)
	{
		model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		tmp = model = glm::translate(model, glm::vec3(x, y, z));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 2.6f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shader.setMat4("model", model);
		mesa.Draw(shader);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 2 / 2.6f));
		model = glm::scale(model, glm::vec3(1 / 0.1f, 1 / 0.1f, 1 / 0.1f));

		tmp = model = glm::translate(model, glm::vec3(-2.0f, 3.78f, 3.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		shader.setMat4("model", model);
		monitor.Draw(shader);

		tmp = model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		monitor.Draw(shader);


		model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		monitor.Draw(shader);

		model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		monitor.Draw(shader);


		tmp = model = glm::scale(model, glm::vec3(1 / 0.7f, 1 / 0.7f, 1 / 0.7f));


		model = glm::translate(model, glm::vec3(-14.0f, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		shader.setMat4("model", model);
		pc.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 0.01f, 1 / 0.01f, 1 / 0.01f));

		model = glm::translate(tmp, glm::vec3(-10.0f, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		shader.setMat4("model", model);
		pc.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 0.01f, 1 / 0.01f, 1 / 0.01f));

		model = glm::translate(tmp, glm::vec3(-5.5f, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		shader.setMat4("model", model);
		pc.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 0.01f, 1 / 0.01f, 1 / 0.01f));

		model = glm::translate(tmp, glm::vec3(-1.5f, 1.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		shader.setMat4("model", model);
		pc.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 0.01f, 1 / 0.01f, 1 / 0.01f));

		model = glm::translate(tmp, glm::vec3(-0.7f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		shader.setMat4("model", model);
		keyboard.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-5.0f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		shader.setMat4("model", model);
		keyboard.Draw(shader);


		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-9.3f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		shader.setMat4("model", model);
		keyboard.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-13.5f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		shader.setMat4("model", model);
		keyboard.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(0.5f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		shader.setMat4("model", model);
		mouse.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-3.7f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		shader.setMat4("model", model);
		mouse.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-7.9f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		shader.setMat4("model", model);
		mouse.Draw(shader);

		model = glm::scale(model, glm::vec3(1 / 2.0f, 1 / 2.0f, 1 / 2.0f));

		model = glm::translate(tmp, glm::vec3(-12.3f, -0.2f, 2.9f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		shader.setMat4("model", model);
		mouse.Draw(shader);

		x += 15;

	}

	model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0 - rotDer, 0.0f, -45.0f + rotIzq));
	tmp2 = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
	tmp3 = glm::rotate(tmp2, glm::radians(rotIzq * 20), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(tmp3, glm::radians(rotDer * 20), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	shader.setMat4("model", model);
	pumpkin.Draw(shader);
	
	//ghost
	model = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
	model = glm::rotate(model, glm::radians(rotRodIzq), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(giroMonito, giroMonito, giroMonito));
	shader.setMat4("model", model);
	ghost.Draw(shader);

	//jotaro
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -35));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	//model = glm::rotate(model, glm::radians(rotRodIzq), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(giroMonito, giroMonito, giroMonito));
	shader.setMat4("model", model);
	jotaro.Draw(shader);

	//proyector

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-5, 23, -35));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(giroMonito, giroMonito, giroMonito));
	shader.setMat4("model", model);
	projector.Draw(shader);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(5, 0, -46));
	model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(giroMonito, giroMonito, giroMonito));
	shader.setMat4("model", model);
	garrafon.Draw(shader);
	


	/*model = glm::translate(model, glm::vec3(-2.0f, 0.0f, -1.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_patron);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
	shader.setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, t_navidad);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/


	glBindVertexArray(0);


	
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto FINAL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, my_input);
	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//To Enable capture of our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glewInit();


	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);

	Shader modelShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	// Load models



	Model mesa = ((char *)"Models/mesa/table2.obj");
	Model monitors = ((char *)"Models/monitor/computer_monitor.obj");
	Model keyboard = ((char *)"Models/keyboard/keyboard.obj");
	Model pc = ((char *)"Models/PC/PC2.obj");
	Model mouse = ((char *)"Models/mouse/mouse.obj");
	Model board = ((char *)"Models/board/board.obj");
	Model ghost = ((char *)"Models/ghost/ghost.obj");
	Model projector = ((char *)"Models/projector/proyector.obj");
	Model fire = ((char *)"Models/fire/fire.obj");
	Model pumpkim = ((char *)"Models/pumpkin/diohead2.obj");
	Model jotaro = ((char *)"Models/jotaro/jotaro2.obj");
	Model garrafon = ((char *)"Models/garrafon/garrafon.obj");
	


	//Inicializaci�n de KeyFrames

	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}

	//declaracion de key frames

	KeyFrame[0].posX = 0.0f;
	KeyFrame[0].posY = -2.0f;
	KeyFrame[0].posZ = -40.0f;
	KeyFrame[0].rotRodIzq = 0.0f;
	KeyFrame[0].giroMonito = 0.0f;

	FrameIndex++;

	KeyFrame[1].posX = 0.0f;
	KeyFrame[1].posY = 15.0f;
	KeyFrame[1].posZ = -40.0f;
	KeyFrame[1].rotRodIzq = 1080.0f;
	KeyFrame[1].giroMonito = 0.08f;

	FrameIndex++;

	KeyFrame[2].posX = 0.0f;
	KeyFrame[2].posY = 15.0f;
	KeyFrame[2].posZ = -60.0f;
	KeyFrame[2].rotRodIzq = 1080.0f;
	KeyFrame[2].giroMonito = 0.08f;

	FrameIndex++;

	KeyFrame[3].posX = 15.0f;
	KeyFrame[3].posY = 15.0f;
	KeyFrame[3].posZ = -40.0f;
	KeyFrame[3].rotRodIzq = 990.0f;
	KeyFrame[3].giroMonito = 0.08f;

	FrameIndex++;

	KeyFrame[4].posX = 0.0f;
	KeyFrame[4].posY = 15.0f;
	KeyFrame[4].posZ = -20.0f;
	KeyFrame[4].rotRodIzq = 900.0f;
	KeyFrame[4].giroMonito = 0.08f;

	FrameIndex++;

	KeyFrame[5].posX = -15.0f;
	KeyFrame[5].posY = 15.0f;
	KeyFrame[5].posZ = -40.0f;
	KeyFrame[5].rotRodIzq = 810.0f;
	KeyFrame[5].giroMonito = 0.08f;

	FrameIndex++;

	KeyFrame[6].posX = 0.0f;
	KeyFrame[6].posY = 15.0f;
	KeyFrame[6].posZ = -60.0f;
	KeyFrame[6].rotRodIzq = 720.0f;
	KeyFrame[6].giroMonito = 0.08f;

	FrameIndex++;

	KeyFrame[7].posX = 0.0f;
	KeyFrame[7].posY = 15.0f;
	KeyFrame[7].posZ = -40.0f;
	KeyFrame[7].rotRodIzq = 720.0f;
	KeyFrame[7].giroMonito = 0.08f;

	FrameIndex++;

	KeyFrame[8].posX = 0.0f;
	KeyFrame[8].posY = -2.0f;
	KeyFrame[8].posZ = -40.0f;
	KeyFrame[8].rotRodIzq = -360.0f;
	KeyFrame[8].giroMonito = 0.0f;

	FrameIndex++;

	printf("ERROR::ASSIMP:: Unable to open file Models / silla / Silla.obj. \n\n");

	for (int i = 0; i < FrameIndex; i++) {
		printf("posX[%i] = %.1f \n", i, KeyFrame[i].posX);
		printf("posY[%i] = %.1f \n", i, KeyFrame[i].posY);
		printf("posZ[%i] = %.1f \n", i, KeyFrame[i].posZ);
		printf("rot[%i] = %.1f  \n", i, KeyFrame[i].rotRodIzq);
		printf("esc[%i] = %.1f \n\n", i, KeyFrame[i].giroMonito);
	}
    
	

	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	// render loop
    // While the windows is not closed
    while (!glfwWindowShouldClose(window))
    {
		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        // input
        // -----
        //my_input(window);
		animate();

        // render
        // Backgound color
        glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//display(modelShader, ourModel, llantasModel);
		display(modelShader, mesa, monitors, pc, keyboard, mouse, 
		board, projector, fire, ghost, pumpkim, jotaro, garrafon);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !calabaza) {

		calabaza = true;

		SoundEngine->play2D("Musica/booo.mp3", GL_TRUE);
		SoundEngine->play2D("Musica/lamento.mp3", GL_TRUE);
		SoundEngine->play2D("Musica/crujido.mp3", GL_TRUE);

	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && calabaza) {

		calabaza = false;

		estado = 0;
		rotIzq = 0.0f;
		rotDer = 0.0f;

	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !calavera) {
		calavera = true;
		SoundEngine->play2D("Musica/jojo.mp3", GL_TRUE);
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && calavera) {
		calavera = false;
		SoundEngine->stopAllSounds();
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !nyancat) {
		nyancat = true;

		SoundEngine->play2D("Musica/Nyancat.mp3", GL_TRUE);
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && nyancat) {
		nyancat = false;
		SoundEngine->stopAllSounds();
	}
	
	
	//To play KeyFrame animation 
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{

		SoundEngine->play2D("Musica/booo.mp3", GL_TRUE);
		SoundEngine->play2D("Musica/lamento.mp3", GL_TRUE);
		SoundEngine->play2D("Musica/crujido.mp3", GL_TRUE);

		if (play == false && (FrameIndex > 1))
		{

			
			resetElements();
			//First Interpolation				
			interpolation();

			

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
	}

	//To Save a KeyFrame
	/*if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}*/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void resize(GLFWwindow* window, int width, int height)
{
    // Set the Viewport to the size of the created window
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}