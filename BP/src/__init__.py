"""
Autor: Samuel Valaštín
Dátum : 28.1.2022
    Inicializacia webovej aplikacie.

"""


from flask import Flask


def init_app():
    """
    Inicializacia aplikacie a registracia blueprintov.
    """
    app = Flask(__name__)
    app.config['SECRET_KEY'] = 'ce7f55b33ef421b188d0cb0cdb8ee563ba6adb6aa39d15f7a0f5cecd3f247fc6'
    from .display import display
    from .analyze import analyze
    from .save import save
    from .mine import mine
    from .detect import detect
    from .stats import stats
    app.register_blueprint(display, url_prefix='/')
    app.register_blueprint(detect, url_prefix='/')
    app.register_blueprint(stats, url_prefix='/')
    app.register_blueprint(analyze, url_prefix='/analyze/')
    app.register_blueprint(save, url_prefix='/save/')
    app.register_blueprint(mine, url_prefix='/mine/')
    return app
