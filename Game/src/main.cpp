#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

#include "Headers/ShadowBox.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// OpenAL include
#include <AL/alut.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para las particulas de la turbina
Shader shaderParticlesLeftThruster;
Shader shaderParticlesRightThruster;
//Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;
//Shader para el menú
Shader shaderMenu;


std::shared_ptr<Camera> camera(new ThirdPersonCamera());
std::shared_ptr<FirstPersonCamera> cameraMenu(new FirstPersonCamera());
float distanceFromTarget = 10;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;

ShadowBox* shadowBox;

// Models complex instances
Model modelCrosshair;
Model modelStart;
Model modelChange;
Model classicSpaceshipSelection;
Model enemySpaceshipSelection;
Model modelHudFullHealth;
Model modelHudFullHealth2;
Model modelHudHalfHealth;
Model modelHudThirdHealth;
Model modelHudThirdHealth2;
Model modelHudNoHealth;
Model modelAsteroid;
Model modelHit;

// Model animate instance
Model spaceshipClassicModelAnimate;
Model spaceshipPurpleModelAnimate;
// Terrain model instance
Terrain terrain(-1, -1, 600, 16, "../Textures/Sheightmap.png");

GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleThrusterID, texId;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/space/blue/bkg1_front.png",
		"../Textures/space/blue/bkg1_back.png",
		"../Textures/space/blue/bkg1_top.png",
		"../Textures/space/blue/bkg1_bot.png",
		"../Textures/space/blue/bkg1_right.png",
		"../Textures/space/blue/bkg1_left.png" };



//spaceship movement variables
float spaceshipRotX = 1.0;
float spaceshipRotY = 1.0;
float spaceshipRotZ = 1.0;
float spaceshipRotXLimit = 0;
float spaceshipRotYLimit = 0;
float spaceshipRotZLimit = 0;
float spaceshipDownMov = 0;
bool downPressed = false;
bool upPressed = false;
bool rightPressed = false;
bool leftPressed = false;
float heightOfTurn = 0;
int numberOfTicksD = 0;
int numberOfTicksU = 0;
int numberOfTicksR = 0;
int numberOfTicksL = 0;
bool accelEnabled = false;
bool brakeEnabled = false;
glm::vec3 thrusterAccelDir = glm::vec3(0.0f,0.01f,0.0f);
bool stopSource1 = false;
float velocity = 0;

//menu options
bool startMenu = true;
bool selectShip = false;
bool mainGame = false;
bool option = false;
bool stopMenuMusic = false;
short int shipSelect=0;
float menuRotation=1.0f;
bool enterPressed = 0;


//HUD & gameplay variables
double deltaCounter = 0;
int secondsCounter = 0;
int animationCounter = 0;
int shipHealth=3;
bool hit = false;
int currentHitMoment = 0;
int lastHitMoment = 0;
bool dontPlay = false;
bool dontPlay2 = false;
bool gameOver = false;

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 modelMatrixSpaceship = glm::mat4(1.0f);
glm::mat4 modelMatrixCrosshair = glm::mat4(1.0f);
glm::mat4 modelMatrixMenu = glm::mat4(1.0f);
glm::mat4 modelMatrixHUD = glm::mat4(1.0f);


int animationIndex = 1;

//vector direction for spaceship
glm::vec3 spaceshipDirection = glm::vec3(-1.0,0.0,0.0);

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
		{"Thruster", glm::vec3(0.0, 0.0, 7.0)},
		{"ThrusterR", glm::vec3(0.0, 0.0, 10.0)},
		{"HUD",glm::vec3(90,15,-100)}
};

//Created with python to ease random calls

std::vector<glm::vec3> asteroidsPos = {
glm::vec3(53.09529829415588,5.411253517807992,-51.55790824756765),
glm::vec3(51.788226581943704,9.242076361200123,-70.4430924859052),
glm::vec3(50.58657875058623,6.540954115076996,-99.44270686143918),
glm::vec3(54.34446922457413,13.685024921754957,-113.42951206438477),
glm::vec3(53.709743555247016,7.64975194672145,-131.71414377407072),
glm::vec3(56.11172047374059,7.450516952922591,-158.84932115594577),
glm::vec3(0.05150201041274727,7.13763291001267,-57.417965919174044),
glm::vec3(1.2894250070164475,10.259038460874978,-78.27454493143136),
glm::vec3(7.79168854970311,6.139354751628952,-97.74076783422554),
glm::vec3(3.0092744319199483,6.6484101138902965,-119.46927745803224),
glm::vec3(6.675004706708805,10.363221540368837,-135.87392527055272),
glm::vec3(3.215598480680171,5.162556847413002,-154.7280355245493),
glm::vec3(-49.357769660779525,11.356163493508598,-50.58586504610972),
glm::vec3(-48.47326178758014,5.514005330231767,-78.49158294983741),
glm::vec3(-46.04413589077008,14.386935918577247,-90.95440255949615),
glm::vec3(-48.75748004222494,6.464646539867202,-115.05012704938538),
glm::vec3(-42.67979060868447,6.539823215079315,-133.65472380980702),
glm::vec3(-44.08626545769671,13.417762342148876,-159.00067959461379),
glm::vec3(-96.09150096738796,12.826727690286724,-57.44491609191819),
glm::vec3(-92.74565427058741,13.626937353394782,-73.04368684876924),
glm::vec3(-97.32734017650802,12.234104895620124,-92.95093689001665),
glm::vec3(-98.33573507792741,9.187046555401569,-111.26079598695603),
glm::vec3(-99.26169318557389,14.698165885286484,-137.30556793451075),
glm::vec3(-94.39762864838589,10.089820567453415,-155.30229424823997),
glm::vec3(-147.91701995813418,10.384378872486526,-53.19928643725163),
glm::vec3(-145.6592316219445,10.41372560537642,-79.77755353015874),
glm::vec3(-148.91692993085138,10.518246116730271,-97.65952387337094),
glm::vec3(-149.51044276586305,14.03090231308671,-118.89236230909302),
glm::vec3(-143.74227633000638,5.740449899841245,-138.10380736917608),
glm::vec3(-141.61899398069932,13.55028898128113,-155.23900125296112),
glm::vec3(-193.7669519729889,14.749439970152844,-53.01520710227659),
glm::vec3(-191.82591904717953,5.94158008760794,-78.91459180310655),
glm::vec3(-197.43211256526104,7.981750436096019,-92.29453614974088),
glm::vec3(-197.70716537560645,13.362448486537026,-118.34048241132149),
glm::vec3(-196.234362537582,5.303638340137281,-132.58349219755095),
glm::vec3(-196.22780088490327,12.60622143328656,-150.76544468073266),
glm::vec3(-241.88655039142674,7.5083340880368805,-55.733720633437215),
glm::vec3(-249.38054360083032,7.120283498447298,-74.76754773328165),
glm::vec3(-242.71829939080067,8.068183052529786,-95.24807008351053),
glm::vec3(-247.27228535324505,12.95802633438036,-111.79227593100185),
glm::vec3(-248.14084546595876,10.33403475243654,-130.1422201206579),
glm::vec3(-241.46371632011665,6.4574415247951595,-157.19836146880957),
glm::vec3(-290.54235088849487,12.871316534012244,-57.237432751748074),
glm::vec3(-293.9639811224496,14.654583470547054,-79.81490497493215),
glm::vec3(-294.6442732927907,8.24042369247977,-97.976251531287),
glm::vec3(-298.82582486322235,11.471136586043738,-118.40395389405899),
glm::vec3(-292.0310476451893,8.796809051911854,-134.0457794078908),
glm::vec3(-294.67534099999506,14.4830871862161,-152.91671929283618),
glm::vec3(-344.53483137071964,10.482116317746744,-55.5498238051141),
glm::vec3(-345.6222120050084,8.42080981388416,-71.55520318706806),
glm::vec3(-344.0366036418258,13.515365341386698,-93.66090802715182),
glm::vec3(-346.8829443938836,10.776589868155188,-111.93252100342072),
glm::vec3(-340.42600289215875,10.709372297502423,-130.15595288816843),
glm::vec3(-346.9587555053882,13.289183287796524,-155.1233347737144),
glm::vec3(-397.98302021287964,9.805388523639005,-54.48663785416705),
glm::vec3(-399.69393457938446,10.803843611648187,-76.74764971022165),
glm::vec3(-399.340731252012,8.951651937975571,-90.09095012984213),
glm::vec3(-398.41322766008955,12.668830677884374,-116.38441971901568),
glm::vec3(-399.39597619163874,12.406387097525839,-139.15802666801756),
glm::vec3(-399.4303999392969,6.180341845149878,-155.33482057677583) };

double deltaTime;
double currTime, lastTime;

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 8000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system Thruster
GLuint initVelThruster, startTimeThruster;
GLuint VAOParticlesThruster;
GLuint nParticlesThruster = 100;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 0.5, particleLifetime = 3.0;
double currTimeParticlesAnimationThruster, lastTimeParticlesAnimationThruster;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 */

// OpenAL Defines
#define NUM_BUFFERS 20
#define NUM_SOURCES 20
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source 2
ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
// Source 3
ALfloat source3Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source3Vel[] = { 0.0, 0.0, 0.0 };
// Source 4
ALfloat source4Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source4Vel[] = { 0.0, 0.0, 0.0 };
//Source 5
ALfloat source5Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source5Vel[] = { 0.0, 0.0, 0.0 };
//Source 6
ALfloat source6Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source6Vel[] = { 0.0, 0.0, 0.0 };
//Source 6
ALfloat source7Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source7Vel[] = { 0.0, 0.0, 0.0 };
//Source 6
ALfloat source8Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source8Vel[] = { 0.0, 0.0, 0.0 };

// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = {true, true, true};

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);
bool isPlaying(ALuint source);
void fillRandomPos(std::vector<glm::vec3> pos , int numEleX,int numEleZ);


void fillRandomPos(std::vector<glm::vec3> pos, int numEleX, int numEleZ) {

	int incX = 0;  
	int incZ = 0;

	for (int i = 0; i++; i < numEleX) {
		incX += 50;
		for (int j = 0; j++; j < numEleZ) {
			pos.push_back(glm::vec3(rand() % 10 - 100 + incX, rand() % 20 + 5, rand()%10 + 40 + incZ));
			incZ += 20;
		}
		incZ = 0;
	}

	

}

bool isPlaying(ALuint source){

	ALenum state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return(state == AL_PLAYING);

}

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat *data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersThruster() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesThruster * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesThruster);
	float rate = particleLifetime / nParticlesThruster;
	for(unsigned int i = 0; i < nParticlesThruster; i++ ) {
		int index = i - nParticlesThruster;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine {};
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER,0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
				glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
				nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderParticlesLeftThruster.initialize("../Shaders/particlesFire.vs", "../Shaders/particlesFire.fs", { "Position", "Velocity", "Age" });
	shaderParticlesRightThruster.initialize("../Shaders/particlesFire.vs", "../Shaders/particlesFire.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");
	shaderMenu.initialize("../Shaders/iluminacion_texture_res.vs", "../Shaders/multipleLights.fs");


	// Inicializacion de los objetos.

	fillRandomPos(asteroidsPos, 12, 6);

	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(100.0f, 100.0f, 100.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	boxLightViewBox.init();
	boxLightViewBox.setShader(&shaderViewDepth);

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	//spaceship
	spaceshipClassicModelAnimate.loadModel("../models/spaceship/nave_ver3.fbx");
	spaceshipClassicModelAnimate.setShader(&shaderMulLighting);

	spaceshipPurpleModelAnimate.loadModel("../models/spaceship/nave_ver3_py.fbx");
	spaceshipPurpleModelAnimate.setShader(&shaderMenu);


	//spaceship Crosshair 
	
	modelCrosshair.loadModel("../models/crosshair/crosshair.fbx");
	modelCrosshair.setShader(&shaderMulLighting);

	//start  Menu
	modelStart.loadModel("../models/menu/start.fbx");
	modelStart.setShader(&shaderMenu);

	modelChange.loadModel("../models/menu/select.fbx");
	modelChange.setShader(&shaderMenu);


	//spaceship selection menu
	classicSpaceshipSelection.loadModel("../models/menu/classicShipSelection.fbx");
	classicSpaceshipSelection.setShader(&shaderMenu);

	enemySpaceshipSelection.loadModel("../models/menu/enemyShipSelection.fbx");
	enemySpaceshipSelection.setShader(&shaderMenu);


	//HUD
	modelHudFullHealth.loadModel("../models/HUD/fullHealth.fbx");
	modelHudFullHealth.setShader(&shaderMulLighting);
	modelHudFullHealth2.loadModel("../models/HUD/fullHealth2.fbx");
	modelHudFullHealth2.setShader(&shaderMulLighting);
	modelHudHalfHealth.loadModel("../models/HUD/halfHealth.fbx");
	modelHudHalfHealth.setShader(&shaderMulLighting);
	modelHudThirdHealth.loadModel("../models/HUD/thirdHealth.fbx");
	modelHudThirdHealth.setShader(&shaderMulLighting);
	modelHudThirdHealth2.loadModel("../models/HUD/thirdHealth2.fbx");
	modelHudThirdHealth2.setShader(&shaderMulLighting);
	modelHudNoHealth.loadModel("../models/HUD/noHealth.fbx");
	modelHudNoHealth.setShader(&shaderMulLighting);

	modelHit.loadModel("../models/HUD/hit.fbx");
	modelHit.setShader(&shaderMulLighting);


	//asteroid

	modelAsteroid.loadModel("../models/asteroid/asteroid.fbx");
	modelAsteroid.setShader(&shaderMulLighting);


	camera->setPosition(glm::vec3(0.0, 0.0, 0.0));
	cameraMenu->setPosition(glm::vec3(0.0, 0.0, 0.0));
	cameraMenu->setFront(glm::vec3(0.0, 0.0, 0.5));
	camera->setFront(glm::vec3(0.0,-1.0,0.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP *bitmap;
	unsigned char *data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth,
				imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

	// Definiendo la textura a utilizar
	Texture textureCesped("../Textures/moon.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureCesped.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureCesped.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureCespedID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/moon3.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/spacet.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/moon5.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/path2.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMap.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);


	Texture textureParticleThruster("../Textures/light.png");
	bitmap = textureParticleThruster.loadImage();
	data = textureParticleThruster.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleThrusterID);
	glBindTexture(GL_TEXTURE_2D, textureParticleThrusterID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticleThruster.freeImage(bitmap);

	std::uniform_real_distribution<float> distr01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesThruster * 2;
	std::vector<GLfloat> randData(size);
	for( int i = 0; i < randData.size(); i++ ) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesLeftThruster.setInt("Pass", 1);
	shaderParticlesLeftThruster.setInt("ParticleTex", 0);
	shaderParticlesLeftThruster.setInt("RandomTex", 1);
	shaderParticlesLeftThruster.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesLeftThruster.setFloat("ParticleSize", particleSize);
	shaderParticlesLeftThruster.setVectorFloat3("Accel", glm::value_ptr(thrusterAccelDir));
	shaderParticlesLeftThruster.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	shaderParticlesRightThruster.setInt("Pass", 1);
	shaderParticlesRightThruster.setInt("ParticleTex", 0);
	shaderParticlesRightThruster.setInt("RandomTex", 1);
	shaderParticlesRightThruster.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesRightThruster.setFloat("ParticleSize", particleSize);
	shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(thrusterAccelDir));
	shaderParticlesRightThruster.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0,1,0);
	n = glm::cross(glm::vec3(1,0,0), v);
	if( glm::length(n) < 0.00001f ) {
		n = glm::cross(glm::vec3(0,1,0), v);
	}
	u = glm::cross(v,n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesLeftThruster.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));
	shaderParticlesRightThruster.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del fuego
	 *******************************************/
	initParticleBuffersThruster();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/accel_long_noisy.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/idle_mono.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/brake_mono.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/turn.wav");
	buffer[4] = alutCreateBufferFromFile("../sounds/menu_change.wav");
	buffer[5] = alutCreateBufferFromFile("../sounds/menu_select.wav");
	buffer[6] = alutCreateBufferFromFile("../sounds/futureBase.wav");
	buffer[7] = alutCreateBufferFromFile("../sounds/Lichtenberg_Figures.wav");
	buffer[8] = alutCreateBufferFromFile("../sounds/hit.wav");
	buffer[9] = alutCreateBufferFromFile("../sounds/warning.wav");
	buffer[10] = alutCreateBufferFromFile("../sounds/noHope.wav");


	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR) {
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}


	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 5.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[2]);
	alSourcei(source[0], AL_LOOPING, 0);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 5.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 2000);

	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 5.0f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, 0);
	alSourcef(source[2], AL_MAX_DISTANCE, 2000);

	alSourcef(source[3], AL_PITCH, 1.0f);
	alSourcef(source[3], AL_GAIN, 5.0f);
	alSourcefv(source[3], AL_POSITION, source3Pos);
	alSourcefv(source[3], AL_VELOCITY, source3Vel);
	alSourcei(source[3], AL_BUFFER, buffer[2]);
	alSourcei(source[3], AL_LOOPING, 0);
	alSourcef(source[3], AL_MAX_DISTANCE, 2000);

	alSourcef(source[4], AL_PITCH, 1.0f);
	alSourcef(source[4], AL_GAIN, 5.0f);
	alSourcefv(source[4], AL_POSITION, source4Pos);
	alSourcefv(source[4], AL_VELOCITY, source4Vel);
	alSourcei(source[4], AL_BUFFER, buffer[2]);
	alSourcei(source[4], AL_LOOPING, 0);
	alSourcef(source[4], AL_MAX_DISTANCE, 2000);

	alSourcef(source[5], AL_PITCH, 1.0f);
	alSourcef(source[5], AL_GAIN, 0.1f);
	alSourcefv(source[5], AL_POSITION, source4Pos);
	alSourcefv(source[5], AL_VELOCITY, source4Vel);
	alSourcei(source[5], AL_BUFFER, buffer[0]);
	alSourcei(source[5], AL_LOOPING, 0);
	alSourcef(source[5], AL_MAX_DISTANCE, 2000);

	alSourcef(source[6], AL_PITCH, 1.0f);
	alSourcef(source[6], AL_GAIN, 1.0f);
	alSourcefv(source[6], AL_POSITION, source5Pos);
	alSourcefv(source[6], AL_VELOCITY, source5Vel);
	alSourcei(source[6], AL_BUFFER, buffer[4]);
	alSourcei(source[6], AL_LOOPING, 0);
	alSourcef(source[6], AL_MAX_DISTANCE, 2000);

	alSourcef(source[7], AL_PITCH, 1.0f);
	alSourcef(source[7], AL_GAIN, 1.0f);
	alSourcefv(source[7], AL_POSITION, source5Pos);
	alSourcefv(source[7], AL_VELOCITY, source5Vel);
	alSourcei(source[7], AL_BUFFER, buffer[5]);
	alSourcei(source[7], AL_LOOPING, 0);
	alSourcef(source[7], AL_MAX_DISTANCE, 2000);

	alSourcef(source[8], AL_PITCH, 1.0f);
	alSourcef(source[8], AL_GAIN, 1.0f);
	alSourcefv(source[8], AL_POSITION, source5Pos);
	alSourcefv(source[8], AL_VELOCITY, source5Vel);
	alSourcei(source[8], AL_BUFFER, buffer[6]);
	alSourcei(source[8], AL_LOOPING, 1);
	alSourcef(source[8], AL_MAX_DISTANCE, 2000);

	alSourcef(source[9], AL_PITCH, 1.0f);
	alSourcef(source[9], AL_GAIN, 1.0f);
	alSourcefv(source[9], AL_POSITION, source4Pos);
	alSourcefv(source[9], AL_VELOCITY, source4Vel);
	alSourcei(source[9], AL_BUFFER, buffer[7]);
	alSourcei(source[9], AL_LOOPING, 1);
	alSourcef(source[9], AL_MAX_DISTANCE, 2000);

	alSourcef(source[10], AL_PITCH, 1.0f);
	alSourcef(source[10], AL_GAIN, 1.0f);
	alSourcefv(source[10], AL_POSITION, source4Pos);
	alSourcefv(source[10], AL_VELOCITY, source4Vel);
	alSourcei(source[10], AL_BUFFER, buffer[8]);
	alSourcei(source[10], AL_LOOPING, 0);
	alSourcef(source[10], AL_MAX_DISTANCE, 2000);

	alSourcef(source[11], AL_PITCH, 1.0f);
	alSourcef(source[11], AL_GAIN, 20.0f);
	alSourcefv(source[11], AL_POSITION, source4Pos);
	alSourcefv(source[11], AL_VELOCITY, source4Vel);
	alSourcei(source[11], AL_BUFFER, buffer[9]);
	alSourcei(source[11], AL_LOOPING, 0);
	alSourcef(source[11], AL_MAX_DISTANCE, 2000);

	alSourcef(source[12], AL_PITCH, 1.0f);
	alSourcef(source[12], AL_GAIN, 1.0f);
	alSourcefv(source[12], AL_POSITION, source4Pos);
	alSourcefv(source[12], AL_VELOCITY, source4Vel);
	alSourcei(source[12], AL_BUFFER, buffer[10]);
	alSourcei(source[12], AL_LOOPING, 0);
	alSourcef(source[12], AL_MAX_DISTANCE, 2000);

}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	shaderParticlesLeftThruster.destroy();
	shaderParticlesRightThruster.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	boxViewDepth.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	modelCrosshair.destroy();
	modelChange.destroy();
	modelStart.destroy();
	classicSpaceshipSelection.destroy();
	enemySpaceshipSelection.destroy();
	modelHudFullHealth.destroy();
	modelHudFullHealth2.destroy();
	modelHudHalfHealth.destroy();
	modelHudThirdHealth.destroy();
	modelHudThirdHealth2.destroy();
	modelHudNoHealth.destroy();
	modelAsteroid.destroy();
	modelHit.destroy();

	// Custom objects animate
	spaceshipClassicModelAnimate.destroy();
	spaceshipPurpleModelAnimate.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleThrusterID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);


}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	/*
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	*/
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
	std::cout << "distance: " << distanceFromTarget << std::endl;
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	if (!gameOver) {
		if (mainGame) {

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
				spaceshipDirection = camera->getFront();
				std::cout << "camera front: (" << spaceshipDirection.x << "," << spaceshipDirection.y << "," << spaceshipDirection.z << ")" << std::endl;
			}
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
				camera->mouseMoveCamera(0.0, offsetY, deltaTime);
				std::cout << "camera pitch: " << camera->getPitch() << std::endl;
			}
			offsetX = 0;
			offsetY = 0;

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				leftPressed = true;
				if (numberOfTicksL < 20) {
					modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(spaceshipRotX), glm::vec3(1, 0, 0));
					spaceshipRotXLimit += spaceshipRotX;
					numberOfTicksL++;
				}
				modelMatrixSpaceship = glm::translate(modelMatrixSpaceship, glm::vec3(0, -0.2, 0.07));

				if (!isPlaying(source[0])) {
					alSourcePlay(source[0]);
				}


			}
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				rightPressed = true;
				if (numberOfTicksR < 20) {
					modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(-spaceshipRotX), glm::vec3(1, 0, 0));
					spaceshipRotXLimit -= spaceshipRotX;
					numberOfTicksR++;
				}
				modelMatrixSpaceship = glm::translate(modelMatrixSpaceship, glm::vec3(0, 0.2, 0.07));


				if (!isPlaying(source[3])) {
					alSourcePlay(source[3]);
				}


			}
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {

				rightPressed = false;
				alSourceStop(source[3]);

			}

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
				leftPressed = false;
				alSourceStop(source[0]);
			}



			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
				modelMatrixSpaceship = glm::translate(modelMatrixSpaceship, glm::vec3(-0.2, 0, 0));
				accelEnabled = true;
				if (!isPlaying(source[5])) {
					alSourcePlay(source[5]);
				}
				std::cout << "ship Position: [ " << modelMatrixSpaceship[3].x << "," << modelMatrixSpaceship[3].y << "," << modelMatrixSpaceship[3].z << " ]" << std::endl;
				//stopSource1 = true;

			}
			else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
				brakeEnabled = true;

			}
			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
				accelEnabled = false;
				alSourceStop(source[5]);
				stopSource1 = false;

			}
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) {
				brakeEnabled = false;
			}


			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
				upPressed = true;
				if (numberOfTicksU < 20) {
					modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(spaceshipRotZ), glm::vec3(0, 1, 0));
					spaceshipRotZLimit += spaceshipRotZ;
					numberOfTicksU++;
				}
				modelMatrixSpaceship = glm::translate(modelMatrixSpaceship, glm::vec3(0, 0, 0.1));

				if (!isPlaying(source[4])) {
					alSourcePlay(source[4]);
				}

			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				downPressed = true;
				if (numberOfTicksD < 20) {
					modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(-spaceshipRotZ), glm::vec3(0, 1, 0));
					spaceshipRotZLimit -= spaceshipRotZ;
					numberOfTicksD++;
				}
				modelMatrixSpaceship = glm::translate(modelMatrixSpaceship, glm::vec3(0, 0, -0.1));

				if (!isPlaying(source[2])) {
					alSourcePlay(source[2]);
				}

			}
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
				downPressed = false;
				heightOfTurn = glm::sin(spaceshipRotZLimit) * 9.87 / numberOfTicksD * 0.429;
				alSourceStop(source[2]);

			}
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
				upPressed = false;
				heightOfTurn = glm::sin(spaceshipRotZLimit) * 9.87 / numberOfTicksU * 0.429;
				alSourceStop(source[4]);
			}


		}

		else if (startMenu) {

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {

				if (option) {
					option = false;
				}

				alSourcePlay(source[6]);

			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {

				if (!option) {
					option = true;
				}

				alSourcePlay(source[6]);
			}

			if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {

				enterPressed = true;

				if (option) {

					startMenu = false;
					mainGame = false;
					selectShip = true;
					alSourcePlay(source[7]);
				}
				else {

					startMenu = false;
					mainGame = true;
					selectShip = false;
					alSourcePlay(source[7]);
				}

			}
		}

		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
			enterPressed = false;
		}

		if (selectShip && !enterPressed) {


			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {

				std::cout << "izq" << std::endl;
				if (shipSelect == 0) {

				}
				else {
					alSourcePlay(source[6]);
					shipSelect--;
				}
			}
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				std::cout << "der" << std::endl;
				if (shipSelect == 0) {
					alSourcePlay(source[6]);
					shipSelect++;
				}
				else {


				}

			}

			if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {

				startMenu = false;
				mainGame = true;
				selectShip = false;
				alSourcePlay(source[7]);

			}
		}

	}
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {



		bool psi = true;

		glm::mat4 view;
		glm::vec3 axis;
		glm::vec3 target;
		float angleTarget;


		modelMatrixSpaceship = glm::translate(modelMatrixSpaceship, glm::vec3(90, 15, -100));
		modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(-90.0f), glm::vec3(1, 0, 0));

		modelMatrixHUD = glm::mat4(modelMatrixSpaceship);

		modelMatrixMenu = glm::translate(modelMatrixMenu, glm::vec3(0, 0, 0));
		modelMatrixMenu = glm::rotate(modelMatrixMenu, glm::radians(180.0f), glm::vec3(0, 1, 0));
		modelMatrixMenu = glm::scale(modelMatrixMenu,glm::vec3(screenWidth/113.0, screenHeight/78.0, 1));

		lastTime = TimeManager::Instance().GetTime();

		// Time for the particles animation
		currTimeParticlesAnimation = lastTime;
		lastTimeParticlesAnimation = lastTime;

		currTimeParticlesAnimationThruster = lastTime;
		lastTimeParticlesAnimationThruster = lastTime;

		glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);

		

			shadowBox = new ShadowBox(-lightPos, camera.get(), 15.0f, 0.1, 45.0f);

			while (psi) {
				currTime = TimeManager::Instance().GetTime();
				if (currTime - lastTime < 0.016666667) {
					glfwPollEvents();
					continue;
				}
				lastTime = currTime;
				TimeManager::Instance().CalculateFrameRate(true);
				deltaTime = TimeManager::Instance().DeltaTime;
				psi = processInput(true);

				deltaCounter += deltaTime;

				if (deltaCounter > 1.0f) {
					secondsCounter++;
					deltaCounter = 0;
				}

				std::map<std::string, bool> collisionDetection;

				// Variables donde se guardan las matrices de cada articulacion por 1 frame
			
				if (mainGame) {
					alSourceStop(source[8]);
					distanceFromTarget = 7;
					camera->setDistanceFromTarget(distanceFromTarget);
					glm::mat4 projection = glm::perspective(glm::radians(45.0f),
						(float)screenWidth / (float)screenHeight, 0.1f, 100.0f);


					axis = glm::axis(glm::quat_cast(modelMatrixSpaceship));
					angleTarget = glm::angle(glm::quat_cast(modelMatrixSpaceship));
					target = modelMatrixSpaceship[3];


					if (std::isnan(angleTarget))
						angleTarget = 0.0;
					if (axis.y < 0)
						angleTarget = -angleTarget;


					camera->setCameraTarget(target);
					camera->setAngleTarget(glm::radians(-90.0f)); //this sets camera front :::: important
					camera->setPitch(0.230067f);
					camera->updateCamera();
					view = camera->getViewMatrix();



					shadowBox->update(screenWidth, screenHeight);


					glm::mat4 lightProjection = glm::mat4(1.0), lightView = glm::mat4(1.0);
					glm::mat4 lightSpaceMatrix;
					lightProjection[0][0] = 2.0 / shadowBox->getWidth();
					lightProjection[1][1] = 2.0 / shadowBox->getHeight();
					lightProjection[2][2] = -2.0 / shadowBox->getLength();
					lightProjection[3][3] = 1.0;
					lightView = glm::lookAt(shadowBox->getCenter(), shadowBox->getCenter() + glm::normalize(-lightPos), glm::vec3(0, 1, 0));

					lightSpaceMatrix = lightProjection * lightView;
					shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));

					// Settea la matriz de vista y projection al shader con solo color
					shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
					shader.setMatrix4("view", 1, false, glm::value_ptr(view));

					// Settea la matriz de vista y projection al shader con skybox
					shaderSkybox.setMatrix4("projection", 1, false,
						glm::value_ptr(projection));
					shaderSkybox.setMatrix4("view", 1, false,
						glm::value_ptr(glm::mat4(glm::mat3(view))));
					// Settea la matriz de vista y projection al shader con multiples luces
					shaderMulLighting.setMatrix4("projection", 1, false,
						glm::value_ptr(projection));
					shaderMulLighting.setMatrix4("view", 1, false,
						glm::value_ptr(view));
					shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
						glm::value_ptr(lightSpaceMatrix));
					// Settea la matriz de vista y projection al shader con multiples luces
					shaderTerrain.setMatrix4("projection", 1, false,
						glm::value_ptr(projection));
					shaderTerrain.setMatrix4("view", 1, false,
						glm::value_ptr(view));
					shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
						glm::value_ptr(lightSpaceMatrix));
					// Settea la matriz de vista y projection al shader para la turbina
					shaderParticlesLeftThruster.setInt("Pass", 2);
					shaderParticlesLeftThruster.setMatrix4("projection", 1, false, glm::value_ptr(projection));
					shaderParticlesLeftThruster.setMatrix4("view", 1, false, glm::value_ptr(view));

					shaderParticlesRightThruster.setInt("Pass", 2);
					shaderParticlesRightThruster.setMatrix4("projection", 1, false, glm::value_ptr(projection));
					shaderParticlesRightThruster.setMatrix4("view", 1, false, glm::value_ptr(view));

					/*******************************************
					 * Propiedades de neblina
					 *******************************************/
					shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.188, 0.098, 0.203)));
					shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.188, 0.098, 0.203)));
					shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.188, 0.098, 0.203)));

					/*******************************************
					 * Propiedades Luz direccional
					 *******************************************/
					shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
					shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
					shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
					shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
					shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

					/*******************************************
					 * Propiedades Luz direccional Terrain
					 *******************************************/
					shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
					shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
					shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
					shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
					shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

					/*******************************************
					 * 1.- We render the depth buffer
					 *******************************************/
					glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					// render scene from light's point of view
					glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
					glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
					glClear(GL_DEPTH_BUFFER_BIT);
					//glCullFace(GL_FRONT);
					prepareDepthScene();
					renderScene(false);
					//glCullFace(GL_BACK);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					/*******************************************
					 * 2.- We render the normal objects
					 *******************************************/
					glViewport(0, 0, screenWidth, screenHeight);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					prepareScene();
					glActiveTexture(GL_TEXTURE10);
					glBindTexture(GL_TEXTURE_2D, depthMap);
					shaderMulLighting.setInt("shadowMap", 10);
					shaderTerrain.setInt("shadowMap", 10);
					/*******************************************
					 * Skybox
					 *******************************************/
					GLint oldCullFaceMode;
					GLint oldDepthFuncMode;
					// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
					glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
					glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
					shaderSkybox.setFloat("skybox", 0);
					glCullFace(GL_FRONT);
					glDepthFunc(GL_LEQUAL);
					glActiveTexture(GL_TEXTURE0);
					skyboxSphere.render();
					glCullFace(oldCullFaceMode);
					glDepthFunc(oldDepthFuncMode);

					renderScene();

					/*******************************************
					 * Creacion de colliders
					 * IMPORTANT do this before interpolations
					 *******************************************/

					 // Collider de spaceship
					AbstractModel::OBB spaceshipCollider;
					glm::mat4 modelmatrixColliderSpaceship = glm::mat4(modelMatrixSpaceship);
					//modelmatrixColliderSpaceship = glm::rotate(modelmatrixColliderSpaceship,glm::radians(90.0f), glm::vec3(1, 0, 0));
					// Set the orientation of collider before doing the scale
					spaceshipCollider.u = glm::quat_cast(modelmatrixColliderSpaceship);
					modelmatrixColliderSpaceship = glm::scale(modelmatrixColliderSpaceship, glm::vec3(4.418, 0.497, 0.237));
					modelmatrixColliderSpaceship = glm::scale(modelmatrixColliderSpaceship, glm::vec3(0.5, 0.5, 0.5));
					modelmatrixColliderSpaceship = glm::translate(modelmatrixColliderSpaceship,
						glm::vec3(spaceshipClassicModelAnimate.getObb().c.x,
							spaceshipClassicModelAnimate.getObb().c.y,
							spaceshipClassicModelAnimate.getObb().c.z));
					spaceshipCollider.e = spaceshipClassicModelAnimate.getObb().e * glm::vec3(4.418, 0.497, 0.237) * glm::vec3(0.5, 0.5, 0.5);
					spaceshipCollider.c = glm::vec3(modelmatrixColliderSpaceship[3]);
					addOrUpdateColliders(collidersOBB, "spaceship", spaceshipCollider, modelMatrixSpaceship);

					//collider de asteroids
					//std::vector<AbstractModel::SBB> asteroidsCollider(asteroidsPos.size());

					for (int i = 0; i < asteroidsPos.size(); i++) {
						AbstractModel::SBB asteroidCollider;
						glm::mat4 modelMatrixColliderAsteroids = glm::mat4(1.0f);
						modelMatrixColliderAsteroids = glm::translate(modelMatrixColliderAsteroids, asteroidsPos[i]);
						modelMatrixColliderAsteroids = glm::scale(modelMatrixColliderAsteroids, glm::vec3(1.0, 1.0, 1.0));

						modelMatrixColliderAsteroids = glm::translate(modelMatrixColliderAsteroids, modelAsteroid.getSbb().c);
						asteroidCollider.c = glm::vec3(modelMatrixColliderAsteroids[3]);
						asteroidCollider.ratio = modelAsteroid.getSbb().ratio * 1.0;
						addOrUpdateColliders(collidersSBB, "asteroid:" + std::to_string(i), asteroidCollider, modelMatrixColliderAsteroids);
					}

					 /*******************************************
					  * Test Colisions
					  *******************************************/
					for (std::map<std::string,
						std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
						collidersOBB.begin(); it != collidersOBB.end(); it++) {
						bool isCollision = false;
						for (std::map<std::string,
							std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
							collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
							if (it != jt
								&& testOBBOBB(std::get<0>(it->second),
									std::get<0>(jt->second))) {
								std::cout << "Colision " << it->first << " with "
									<< jt->first << std::endl;
								isCollision = true;
							}
						}
						addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
					}

					for (std::map<std::string,
						std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
						collidersSBB.begin(); it != collidersSBB.end(); it++) {
						bool isCollision = false;
						for (std::map<std::string,
							std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt =
							collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
							if (it != jt
								&& testSphereSphereIntersection(std::get<0>(it->second),
									std::get<0>(jt->second))) {
								std::cout << "Colision " << it->first << " with "
									<< jt->first << std::endl;
								isCollision = true;
							}
						}
						addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
					}

					for (std::map<std::string,
						std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
						collidersSBB.begin(); it != collidersSBB.end(); it++) {
						bool isCollision = false;
						std::map<std::string,
							std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
							collidersOBB.begin();
						for (; jt != collidersOBB.end(); jt++) {
							if (testSphereOBox(std::get<0>(it->second),
								std::get<0>(jt->second))) {
								std::cout << "Colision " << it->first << " with "
									<< jt->first << std::endl;
								isCollision = true;
								addOrUpdateCollisionDetection(collisionDetection, jt->first, isCollision);
							}
						}
						addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
					}

					std::map<std::string, bool>::iterator colIt;
					for (colIt = collisionDetection.begin(); colIt != collisionDetection.end();
						colIt++) {
						std::map<std::string,
							std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
							collidersSBB.find(colIt->first);
						std::map<std::string,
							std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
							collidersOBB.find(colIt->first);
						if (it != collidersSBB.end()) {
							if (!colIt->second)
								addOrUpdateColliders(collidersSBB, it->first);
						}
						if (jt != collidersOBB.end()) {
							if (!colIt->second)
								addOrUpdateColliders(collidersOBB, jt->first);
							else {

								if (jt->first.compare("spaceship") == 0) {
									currentHitMoment = secondsCounter;
									modelMatrixSpaceship = std::get<1>(jt->second);
									if (currentHitMoment - lastHitMoment > 2) {
										shipHealth--;
										hit = true;
									}
									else {
										hit = false;
										dontPlay = false;
									}

									lastHitMoment = currentHitMoment;
								}
							}
						}
					}

					/*******************************************
					 * State machines
					 *******************************************/


					glfwSwapBuffers(window);


					if (!downPressed && numberOfTicksD != 0) {
						if (spaceshipRotZLimit < 0) {
							modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(spaceshipRotZ), glm::vec3(0, 1, 0));
							modelMatrixSpaceship = glm::translate(modelMatrixSpaceship, glm::vec3(0, 0, -0.1));
							//modelMatrixHUD = glm::translate(modelMatrixHUD, glm::vec3(0, 0, -0.1));
							spaceshipRotZLimit += spaceshipRotZ;
							numberOfTicksD--;
						}

					}
					if (!upPressed && numberOfTicksU != 0) {
						if (spaceshipRotZLimit > 0) {
							modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(-spaceshipRotZ), glm::vec3(0, 1, 0));
							modelMatrixSpaceship = glm::translate(modelMatrixSpaceship, glm::vec3(0, 0, 0.1));
							//modelMatrixHUD = glm::translate(modelMatrixHUD, glm::vec3(0, 0, 0.1));
							spaceshipRotZLimit -= spaceshipRotZ;
							numberOfTicksU--;
						}

					}

					if (!rightPressed && numberOfTicksR != 0) {
						if (spaceshipRotXLimit < 0) {
							modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(spaceshipRotX), glm::vec3(1, 0, 0));
							spaceshipRotXLimit += spaceshipRotX;
							numberOfTicksR--;
						}

					}

					if (!leftPressed && numberOfTicksL != 0) {
						if (spaceshipRotXLimit > 0) {
							modelMatrixSpaceship = glm::rotate(modelMatrixSpaceship, glm::radians(-spaceshipRotX), glm::vec3(1, 0, 0));
							spaceshipRotXLimit -= spaceshipRotX;
							numberOfTicksL--;
						}

					}



					/****************************+
					 * Open AL sound data
					 */


					source0Pos[0] = modelMatrixSpaceship[3].x;
					source0Pos[1] = modelMatrixSpaceship[3].y;
					source0Pos[2] = modelMatrixSpaceship[3].z;
					alSourcefv(source[0], AL_POSITION, source0Pos);

					source2Pos[0] = modelMatrixSpaceship[3].x;
					source2Pos[1] = modelMatrixSpaceship[3].y;
					source2Pos[2] = modelMatrixSpaceship[3].z;
					alSourcefv(source[2], AL_POSITION, source2Pos);

					source3Pos[0] = modelMatrixSpaceship[3].x;
					source3Pos[1] = modelMatrixSpaceship[3].y;
					source3Pos[2] = modelMatrixSpaceship[3].z;
					alSourcefv(source[3], AL_POSITION, source3Pos);

					source4Pos[0] = modelMatrixSpaceship[3].x;
					source4Pos[1] = modelMatrixSpaceship[3].y;
					source4Pos[2] = modelMatrixSpaceship[3].z;
					alSourcefv(source[4], AL_POSITION, source4Pos);

					alSourcefv(source[5], AL_POSITION, source4Pos);

					alSourcefv(source[9], AL_POSITION, source4Pos);
					alSourcefv(source[10], AL_POSITION, source4Pos);
					alSourcefv(source[11], AL_POSITION, source4Pos);
					alSourcefv(source[12], AL_POSITION, source4Pos);

					// Listener for the Thris person camera
					listenerPos[0] = modelMatrixSpaceship[3].x;
					listenerPos[1] = modelMatrixSpaceship[3].y;
					listenerPos[2] = modelMatrixSpaceship[3].z;
					alListenerfv(AL_POSITION, listenerPos);

					glm::vec3 upModel = glm::normalize(modelMatrixSpaceship[1]);
					glm::vec3 frontModel = glm::normalize(modelMatrixSpaceship[2]);

					listenerOri[0] = frontModel.x;
					listenerOri[1] = frontModel.y;
					listenerOri[2] = frontModel.z;
					listenerOri[3] = upModel.x;
					listenerOri[4] = upModel.y;
					listenerOri[5] = upModel.z;

					alListenerfv(AL_ORIENTATION, listenerOri);

					if (!gameOver) {
						if (!stopSource1) {
							if (!isPlaying(source[1])) {

								alSourcePlay(source[1]);
							}
						}
						else alSourceStop(source[1]);


						if (!isPlaying(source[9])) {
							alSourcePlay(source[9]);
						}
				
					}
					else {
						alSourceStop(source[1]);
						alSourceStop(source[9]);
						if (!isPlaying(source[12])) {

							alSourcePlay(source[12]);
						}
					}
				}
				else if (startMenu) {
					distanceFromTarget = 10;
					camera->setDistanceFromTarget(distanceFromTarget);
					glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
					glViewport(0, 0, screenWidth, screenHeight);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glm::mat4 view;
					glm::vec3 axis;
					glm::vec3 target;
					float angleTarget;

					glm::mat4 projection = glm::perspective(glm::radians(45.0f),
						(float)screenWidth / (float)screenHeight, 0.01f, 100.0f);

					

					axis = glm::axis(glm::quat_cast(modelMatrixMenu));
					angleTarget = glm::angle(glm::quat_cast(modelMatrixMenu));
					target = modelMatrixMenu[3];


					camera->setCameraTarget(target);
					camera->setAngleTarget(glm::radians(0.0f)); //this sets camera front :::: important
					camera->setPitch(0.f);
					camera->updateCamera();
					view = camera->getViewMatrix();


					shaderMenu.setMatrix4("projection", 1, false,
						glm::value_ptr(projection));
					shaderMenu.setMatrix4("view", 1, false,
						glm::value_ptr(view));


					shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
					shader.setMatrix4("view", 1, false, glm::value_ptr(view));

					shaderMenu.setMatrix4("projection", 1, false,
						glm::value_ptr(projection));
					shaderMenu.setMatrix4("view", 1, false,
						glm::value_ptr(view));

					shaderMenu.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
					shaderMenu.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
					shaderMenu.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
					shaderMenu.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
					shaderMenu.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

					shaderMenu.setInt("spotLightCount", 0);
					shaderMenu.setInt("pointLightCount", 0);

					if (!option) {
						modelStart.render(modelMatrixMenu);
					}
					else
						modelChange.render(modelMatrixMenu);


					//OPEN AL

					source5Pos[0] = camera->getPosition().x;
					source5Pos[1] = camera->getPosition().y;
					source5Pos[2] = camera->getPosition().z;

					alSourcefv(source[6], AL_POSITION, source5Pos);

					alSourcefv(source[7], AL_POSITION, source5Pos);

					alSourcefv(source[8], AL_POSITION, source5Pos);

					// Listener for the Third person camera
					listenerPos[0] = camera->getPosition().x;
					listenerPos[1] = camera->getPosition().y;
					listenerPos[2] = camera->getPosition().z;
					alListenerfv(AL_POSITION, listenerPos);

					if (!stopMenuMusic) {
						if (!isPlaying(source[8])) {
							alSourcePlay(source[8]);
						}
					}
					else alSourceStop(source[8]);


					glfwSwapBuffers(window);
				}

			

				else if (selectShip) {

					spaceshipClassicModelAnimate.setShader(&shaderMenu);
					distanceFromTarget = 25;
					camera->setDistanceFromTarget(distanceFromTarget);
					glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
					glViewport(0, 0, screenWidth, screenHeight);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glm::mat4 view;
					glm::vec3 axis;
					glm::vec3 target;
					float angleTarget;

					glm::mat4 projection = glm::perspective(glm::radians(45.0f),
						(float)screenWidth / (float)screenHeight, 0.01f, 100.0f);

					
					glm::mat4 modelMatrixSelectionClassic = glm::mat4(modelMatrixSpaceship);
					modelMatrixSelectionClassic = glm::rotate(modelMatrixSelectionClassic, glm::radians(-30.0f), glm::vec3(0, 1, 0));
					modelMatrixSelectionClassic = glm::rotate(modelMatrixSelectionClassic,glm::radians(menuRotation),glm::vec3(0,0,1));
					modelMatrixSelectionClassic = glm::scale(modelMatrixSelectionClassic, glm::vec3(4.418, 0.497, 0.237));
					menuRotation += 1.0f;

					
					glm::mat4 modelMatrixHUD = glm::mat4(modelMatrixSpaceship);
					modelMatrixHUD = glm::translate(modelMatrixHUD, glm::vec3(-13,0,0));
					modelMatrixHUD = glm::rotate(modelMatrixHUD, glm::radians(-90.0f), glm::vec3(0, 1, 0));
					modelMatrixHUD = glm::rotate(modelMatrixHUD, glm::radians(-90.0f), glm::vec3(0, 0, 1));
					modelMatrixHUD = glm::scale(modelMatrixHUD, glm::vec3(10, 10, 10));

					axis = glm::axis(glm::quat_cast(modelMatrixSelectionClassic));
					angleTarget = glm::angle(glm::quat_cast(modelMatrixSelectionClassic));
					target = modelMatrixSelectionClassic[3];


					camera->setCameraTarget(target);
					camera->setAngleTarget(glm::radians(90.0f)); //this sets camera front :::: important
					//camera->setPitch(0.f);
					camera->updateCamera();
					view = camera->getViewMatrix();


					shaderMenu.setMatrix4("projection", 1, false,
						glm::value_ptr(projection));
					shaderMenu.setMatrix4("view", 1, false,
						glm::value_ptr(view));


					shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
					shader.setMatrix4("view", 1, false, glm::value_ptr(view));

					shaderMenu.setMatrix4("projection", 1, false,
						glm::value_ptr(projection));
					shaderMenu.setMatrix4("view", 1, false,
						glm::value_ptr(view));

					shaderMenu.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
					shaderMenu.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(1, 1, 1)));
					shaderMenu.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
					shaderMenu.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
					shaderMenu.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

					shaderMenu.setInt("spotLightCount", 0);
					shaderMenu.setInt("pointLightCount", 0);

					if(shipSelect==0){
						spaceshipClassicModelAnimate.render(modelMatrixSelectionClassic);
						classicSpaceshipSelection.render(modelMatrixHUD);
						
					}
					else {
						spaceshipPurpleModelAnimate.render(modelMatrixSelectionClassic);
						enemySpaceshipSelection.render(modelMatrixHUD);
					}



					



					//OPEN AL

					source5Pos[0] = camera->getPosition().x;
					source5Pos[1] = camera->getPosition().y;
					source5Pos[2] = camera->getPosition().z;

					alSourcefv(source[6], AL_POSITION, source5Pos);

					alSourcefv(source[7], AL_POSITION, source5Pos);

					alSourcefv(source[8], AL_POSITION, source5Pos);

					// Listener for the Third person camera
					listenerPos[0] = camera->getPosition().x;
					listenerPos[1] = camera->getPosition().y;
					listenerPos[2] = camera->getPosition().z;

					alListenerfv(AL_POSITION, listenerPos);

					if (!stopMenuMusic) {
						if (!isPlaying(source[8])) {
							alSourcePlay(source[8]);
						}
					}
					else alSourceStop(source[8]);



					glfwSwapBuffers(window);
				}

			}

}

void prepareScene(){

	skyboxSphere.setShader(&shaderSkybox);

	terrain.setShader(&shaderTerrain);

	spaceshipClassicModelAnimate.setShader(&shaderMulLighting);

	spaceshipPurpleModelAnimate.setShader(&shaderMulLighting);

	modelCrosshair.setShader(&shaderMulLighting);

	modelHudFullHealth.setShader(&shaderMulLighting);

	modelHudFullHealth2.setShader(&shaderMulLighting);

	modelHudHalfHealth.setShader(&shaderMulLighting);
	
	modelHudThirdHealth.setShader(&shaderMulLighting);
	
	modelHudThirdHealth2.setShader(&shaderMulLighting);

	modelHudNoHealth.setShader(&shaderMulLighting);

	modelAsteroid.setShader(&shaderMulLighting);
}

void prepareDepthScene(){

	skyboxSphere.setShader(&shaderDepth);

	terrain.setShader(&shaderDepth);

	spaceshipClassicModelAnimate.setShader(&shaderDepth);

	spaceshipPurpleModelAnimate.setShader(&shaderDepth);

	modelAsteroid.setShader(&shaderDepth);

}

void renderScene(bool renderParticles){
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	/*******************************************
	 * Custom objects obj
	 *******************************************/

	glm::mat4 modelMatrixCrosshair = glm::mat4(modelMatrixSpaceship);
	
	modelMatrixCrosshair = glm::translate(modelMatrixCrosshair, glm::vec3(-15, 0, 0));
	modelMatrixCrosshair = glm::rotate(modelMatrixCrosshair, glm::radians(90.0f), glm::vec3(0, 1, 0));
	modelMatrixCrosshair = glm::scale(modelMatrixCrosshair, glm::vec3(3,3,3));
	modelCrosshair.render(modelMatrixCrosshair);
	glm::mat4 modelMatrixCrosshairFar = glm::mat4(modelMatrixSpaceship);
	modelMatrixCrosshairFar = glm::translate(modelMatrixCrosshairFar, glm::vec3(-20, 0, 0));
	modelMatrixCrosshairFar = glm::rotate(modelMatrixCrosshairFar, glm::radians(90.0f), glm::vec3(0, 1, 0));
	modelMatrixCrosshairFar = glm::rotate(modelMatrixCrosshairFar, glm::radians(90.0f), glm::vec3(0, 1, 0));
	modelMatrixCrosshairFar = glm::scale(modelMatrixCrosshairFar, glm::vec3(2, 2, 2));
	modelCrosshair.render(modelMatrixCrosshairFar);


	/*******************************************
	 * Custom Anim objects obj
	 *******************************************/

	glColor3f(1, 1, 1);

	if (modelMatrixSpaceship[3][1] < terrain.getHeightTerrain(modelMatrixSpaceship[3][0], modelMatrixSpaceship[3][2]) + 3) {
		modelMatrixSpaceship[3][1] = terrain.getHeightTerrain(modelMatrixSpaceship[3][0], modelMatrixSpaceship[3][2]) + 3;
	}


	glm::mat4 modelMatrixSpaceshipBody = glm::mat4(modelMatrixSpaceship);
	//modelMatrixSpaceshipBody = glm::rotate(modelMatrixSpaceshipBody, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	modelMatrixSpaceshipBody = glm::scale(modelMatrixSpaceshipBody, glm::vec3(4.418, 0.497, 0.237));
	modelMatrixSpaceshipBody = glm::scale(modelMatrixSpaceshipBody, glm::vec3(0.5, 0.5, 0.5));
	spaceshipClassicModelAnimate.setAnimationIndex(0);
	if (shipSelect == 0) {
		spaceshipClassicModelAnimate.render(modelMatrixSpaceshipBody);
	}else 
		spaceshipPurpleModelAnimate.render(modelMatrixSpaceshipBody);


	for (int i = 0; i < asteroidsPos.size(); i++) {
		if(asteroidsPos[i].y < terrain.getHeightTerrain(asteroidsPos[i].x, asteroidsPos[i].z))
			asteroidsPos[i].y = terrain.getHeightTerrain(asteroidsPos[i].x, asteroidsPos[i].z);
		modelAsteroid.setPosition(asteroidsPos[i]);
		modelAsteroid.setScale(glm::vec3(1, 1, 1));
		//modelAsteroid.setOrientation(glm::vec3(0, lamp1Orientation[i], 0));
		modelAsteroid.render();
	}



	/**********
	 * Update the position with alpha objects
	 */

	blendingUnsorted.find("spaceship")->second = glm::vec3(modelMatrixSpaceship[3]);
	blendingUnsorted.find("HUD")->second = glm::vec3(modelMatrixHUD[3]);
	/**********
	 * Sorter with alpha objects
	 */
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for(itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end(); itblend++){
		float distanceFromView = glm::length(camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first, itblend->second);
	}

	/**********
	 * Render de las transparencias
	 */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for(std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it = blendingSorted.rbegin(); it != blendingSorted.rend(); it++){
		 if (it->second.first.compare("HUD") == 0) {
			 
			 modelMatrixHUD[3] = modelMatrixSpaceship[3];
			 glm::mat4 modelMatrixHUD2 = glm::mat4(modelMatrixHUD);
			 modelMatrixHUD2 = glm::rotate(modelMatrixHUD2, glm::radians(90.0f) - 0.230067f, glm::vec3(0, 1, 0));
			 modelMatrixHUD2 = glm::rotate(modelMatrixHUD2, glm::radians(90.0f), glm::vec3(0, 0, 1));
			 modelMatrixHUD2 = glm::scale(modelMatrixHUD2, glm::vec3(5, 5.8, 1));
			 if (shipHealth == 3) {
				 if (secondsCounter % 2 != 0)
					 modelHudFullHealth.render(modelMatrixHUD2);
				 else
					 modelHudFullHealth2.render(modelMatrixHUD2);
			 }
			 else if (shipHealth == 2) {
				 modelHudHalfHealth.render(modelMatrixHUD2);
			 }
			 else if (shipHealth == 1) {
				 if (!dontPlay2) {
					 if(!isPlaying(source[11]))
						alSourcePlay(source[11]);
					 dontPlay2 = true;
				 }
				 if (secondsCounter % 2 != 0)
					 modelHudThirdHealth.render(modelMatrixHUD2);
				 else
					 modelHudThirdHealth2.render(modelMatrixHUD2);
			 }
			 else {
				 modelHudNoHealth.render(modelMatrixHUD2);
				 gameOver = true;
			 }





			 glm::mat4 modelMatrixHUD3 = glm::mat4(modelMatrixHUD2);
			 modelMatrixHUD3 = glm::translate(modelMatrixHUD3, glm::vec3(0, 0, 5));
			 modelHit.setAnimationIndex(0);
			 if (hit) {
				 animationCounter = secondsCounter;
				 if (animationCounter - lastHitMoment < 1) {
					 modelHit.render(modelMatrixHUD3);
					 if (!isPlaying(source[10]) && !dontPlay) {
						 alSourcePlay(source[10]);
						 dontPlay = true;
					 }
				 }
				 else {

				 }

			 }

		 }


		else if(renderParticles && it->second.first.compare("Thruster") == 0){
			/**********
			 * Init Render particles systems
			 */
			lastTimeParticlesAnimationThruster = currTimeParticlesAnimationThruster;
			currTimeParticlesAnimationThruster = TimeManager::Instance().GetTime();

			shaderParticlesLeftThruster.setInt("Pass", 1);

			//Thruster particles animation affected by the velocity of the ship

			if (accelEnabled) {

				if (leftPressed) {
					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(100, 0, 0)));
				}
				else if (rightPressed) {
					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(-100, 0, 0)));

				} 
				else if (upPressed) {

					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 0, 100)));

				}
				else if (downPressed) {

					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 0, -100)));

				}
				else shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 10, 0)));



			}
			else if (brakeEnabled) {
				if (leftPressed) {
					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(1.0, 0, 0)));
				}
				else if (rightPressed) {
					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(-1.0, 0, 0)));
				}
				else if (upPressed) {

					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 0, 1.0)));

				}
				else if (downPressed) {

					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 0, -1.0)));

				}
				else shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 0.1, 0)));
			}
			else {


				if (leftPressed) {
					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(10, 0, 0)));
				}
				else if (rightPressed) {
					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(-10, 0, 0)));
				}
				else if (upPressed) {

					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 0, 10)));

				}
				else if (downPressed) {

					shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 0, -10)));

				}
				else shaderParticlesRightThruster.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0, 1.0, 0)));

			}


			shaderParticlesLeftThruster.setFloat("Time", currTimeParticlesAnimationThruster);
			shaderParticlesLeftThruster.setFloat("DeltaT", currTimeParticlesAnimationThruster - lastTimeParticlesAnimationThruster);


			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, texId);
			glEnable(GL_RASTERIZER_DISCARD);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			glBeginTransformFeedback(GL_POINTS);
			glBindVertexArray(particleArray[1-drawBuf]);
			glVertexAttribDivisor(0,0);
			glVertexAttribDivisor(1,0);
			glVertexAttribDivisor(2,0);
			glDrawArrays(GL_POINTS, 0, nParticlesThruster);
			glEndTransformFeedback();
			glDisable(GL_RASTERIZER_DISCARD);

			shaderParticlesLeftThruster.setInt("Pass", 2);
			glm::mat4 modelThrusterParticles = glm::mat4(modelMatrixSpaceshipBody);
			modelThrusterParticles = glm::translate(modelThrusterParticles, glm::vec3(0.005856, -1.83928 , -0.550695));
			modelThrusterParticles = glm::rotate(modelThrusterParticles, glm::radians(90.0f), glm :: vec3(0, 0, 1));
			modelThrusterParticles = glm::rotate(modelThrusterParticles, glm::radians(180.0f), glm::vec3(1, 0, 0 ));
			shaderParticlesLeftThruster.setMatrix4("model", 1, false, glm::value_ptr(modelThrusterParticles));



			shaderParticlesLeftThruster.turnOn();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleThrusterID);
			glDepthMask(GL_FALSE);
			glBindVertexArray(particleArray[drawBuf]);
			glVertexAttribDivisor(0,1);
			glVertexAttribDivisor(1,1);
			glVertexAttribDivisor(2,1);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesThruster);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
			drawBuf = 1 - drawBuf;
			shaderParticlesLeftThruster.turnOff();



			/****************************+
			 * Open AL sound data
			 */

			source1Pos[0] = modelMatrixSpaceship[3].x;
			source1Pos[1] = modelMatrixSpaceship[3].y;
			source1Pos[2] = modelMatrixSpaceship[3].z;
			alSourcefv(source[1], AL_POSITION, source1Pos);

			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("ThrusterR") == 0) {

		lastTimeParticlesAnimationThruster = currTimeParticlesAnimationThruster;
		currTimeParticlesAnimationThruster = TimeManager::Instance().GetTime();


		shaderParticlesRightThruster.setInt("Pass", 1);
		shaderParticlesRightThruster.setFloat("Time", currTimeParticlesAnimationThruster);
		shaderParticlesRightThruster.setFloat("DeltaT", currTimeParticlesAnimationThruster - lastTimeParticlesAnimationThruster);


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, texId);
		glEnable(GL_RASTERIZER_DISCARD);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
		glBeginTransformFeedback(GL_POINTS);
		glBindVertexArray(particleArray[1 - drawBuf]);
		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 0);
		glVertexAttribDivisor(2, 0);
		glDrawArrays(GL_POINTS, 0, nParticlesThruster);
		glEndTransformFeedback();
		glDisable(GL_RASTERIZER_DISCARD);


		shaderParticlesRightThruster.setInt("Pass", 2);
		glm::mat4 modelThrusterParticlesRight = glm::mat4(modelMatrixSpaceshipBody);
		modelThrusterParticlesRight = glm::translate(modelThrusterParticlesRight, glm::vec3(0.005856, 1.502, -0.550695));
		modelThrusterParticlesRight = glm::rotate(modelThrusterParticlesRight, glm::radians(90.0f), glm::vec3(0, 0, 1));
		modelThrusterParticlesRight = glm::rotate(modelThrusterParticlesRight, glm::radians(180.0f), glm::vec3(1, 0, 0));
		shaderParticlesRightThruster.setMatrix4("model", 1, false, glm::value_ptr(modelThrusterParticlesRight));

		shaderParticlesRightThruster.turnOn();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureParticleThrusterID);
		glDepthMask(GL_FALSE);
		glBindVertexArray(particleArray[drawBuf]);
		glVertexAttribDivisor(0, 1);
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesThruster);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		drawBuf = 1 - drawBuf;
		shaderParticlesRightThruster.turnOff();

		}

	}
	glEnable(GL_CULL_FACE);
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
