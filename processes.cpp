#include <SFML/Graphics.hpp>
#include <sstream>
#include <string.h>
#include <iostream>

class server;
class wrapper;
class windowWrapper;

class Button {

private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color clickColor;
    sf :: Font font;
    bool isClicked;
public:
    Button(float x, float y, float width, float height, sf::Color idleColor, sf::Color hoverColor, sf::Color clickColor,std::string text) :
        idleColor(idleColor), hoverColor(hoverColor), clickColor(clickColor) {
        shape.setPosition(sf::Vector2f(x, y));
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(idleColor);
        
        if(!font.loadFromFile("Arial.ttf")){
        	perror("failed to load from file");
        	exit(1);
        }
        this->text = sf::Text(text,font,20);
        this->text.setCharacterSize(height-10);
        this->text.move(10,0);
        this->text.setPosition(x,y);
    }

    void update(wrapper*,bool);
    
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
    
    sf::RectangleShape& getShape(){
    	return this->shape;
    }

};


class windowWrapper{

	sf :: RenderWindow window;
	sf :: Font font;
	sf :: Text* text;
	std :: string inputText;
	std :: vector<sf :: Text*> lines;
	Button button,recvButton;
	sf :: RectangleShape drawingLine;
		
	public:
	
	windowWrapper(std :: string processName) : window(sf::VideoMode(800, 600), processName), button(725,10,60,30,sf::Color(100, 100, 100), sf::Color(150, 150, 150), sf::Color(50, 50, 50)," send"),recvButton(725-button.getShape().getGlobalBounds().width-30,10,80,30,sf::Color(100, 100, 100), sf::Color(150, 150, 150), sf::Color(50, 50, 50)," receive"){
	
		init();
	}
	
	void init(){
    		
    		if (!font.loadFromFile("Arial.ttf")) {
        		std::cerr << "Error loading font file" << std::endl;
        		exit(1);
    		}
	
		drawingLine.setPosition(sf::Vector2f(0,button.getShape().getGlobalBounds().height + 20));
		drawingLine.setSize(sf::Vector2f(800,5));
		drawingLine.setFillColor(sf::Color::White);

    		text = new sf::Text("", font, 20);
    		text->setPosition(10, drawingLine.getPosition().y + drawingLine.getGlobalBounds().height + 20);
    		
    		lines.push_back(text);
    
	}
	
	void render(wrapper*);
	
	void innerRender(){
	
	sf::Event event;
        	while (window.pollEvent(event))
        	{
        	    if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
        	        window.close();
	
        	    if (event.type == sf::Event::TextEntered)
        	    {
        	        if (event.text.unicode == '\b') // Backspace
        	        {
        	            if (!inputText.empty()){
        	                inputText.pop_back();
        	                }
        	            else if(!lines.empty() && inputText.empty() && lines.size() > 1)
        	            	{
        	            	    delete text;
        	            	    text = nullptr;

        	            	    if(lines.size() > 1)
        	            	    {
        	            	       text = lines[lines.size()-2];
        	                       lines.pop_back();
        	              	       inputText = text->getString();
        	                    }

        	        	}
        	        }
        	        else if(event.text.unicode == 13){

			    float val = text->getPosition().y + text->getGlobalBounds().height + 10;
			    
        	            
        	            text = nullptr;
        	            text = new sf::Text("",font,20);
        	            text->move(10,val);
        	            lines.push_back(text);
        	            inputText = "";
        	        }
        	        else if (event.text.unicode < 128)
        	        {
        	            inputText += static_cast<char>(event.text.unicode);
        	        }
	
        	        text->setString(inputText);
        	     
        	}
	

        	window.clear();
		button.draw(window);
        	recvButton.draw(window);
        	window.draw(drawingLine);
        	
        	for (const auto& line : lines)
        	    window.draw(*line);
        	window.display();
        	
        	}
        	
        	button.draw(window);
        	recvButton.draw(window);
        	window.draw(drawingLine);
     		window.display();
	
	}
	
	char* getText(){
		std :: string str = "";
		for(std :: vector<sf::Text*> :: iterator it = lines.begin();it != lines.end() ; it++){
			str += (*it)->getString();
			str += '\n';
		}

		str += '\0';
				
		int n = str.length();
		char* ptr = new char[n];
		
		strcpy(ptr,str.c_str());
		return ptr;
	}
	
	sf :: RenderWindow& getWindow() { return this->window; }
	void setText(std :: string str){
		clearText();
		this->text->setString(str);
		this->inputText = str;
	}
	void setText(char* str){
		clearText();
		this->text->setString(std :: string(str));
		this->inputText = str;
	}
	void clearText(){
		
		for(const auto& line : lines) delete line;
		
		while(!lines.empty()) lines.pop_back();
		this->text = new sf :: Text("",font,20);
 		text->setPosition(10, drawingLine.getPosition().y + drawingLine.getGlobalBounds().height + 20);
		this->text->setString("");
		this->inputText = "";
		lines.push_back(text);
	}
		
};
