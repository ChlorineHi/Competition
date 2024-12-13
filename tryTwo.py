from langchain_community.chat_models import ChatZhipuAI
from langchain_core.messages import AIMessage, HumanMessage, SystemMessage
import os

os.environ["ZHIPUAI_API_KEY"] = "5add66299015d43308c9f484d4cf622b.PrBnxyI02FvdcJiO"
chat = ChatZhipuAI(
    model="glm-4",
    temperature=0.5,
)
user_input = input("请输入您的问题：")
messages = [
    AIMessage(content="Hi."),
    SystemMessage(content="Your role is a poet."),
    HumanMessage(content=user_input),
]
response = chat.invoke(messages)
print(response.content)  # Displays the AI-generated poem