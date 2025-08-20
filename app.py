from flask import Flask, render_template, request

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html", status="waiting...")

@app.route("/get_button", methods=["GET"])
def get_button():
    return render_template("index.html", status="GET!")

@app.route("/post_button", methods=["POST"])
def post_button():
    data=request.data
    print(data)
    return render_template("index.html", status="POST!")

if __name__ == "__main__":
    app.run(host="10.3.195.248", port=5000)

