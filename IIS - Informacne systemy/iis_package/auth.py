from flask import Blueprint, render_template, request, flash, redirect, url_for, jsonify, session
from . import db
from .models import Role, User, UserRoles
from werkzeug.security import generate_password_hash, check_password_hash
from flask_login import login_user, login_required, logout_user, current_user
from functools import wraps
import time

auth = Blueprint("auth", __name__)


def user_privileges(user_role):
    def decorator(func):
        @wraps(func)
        def wrapped_function(*args, **kwargs):
            if not current_user.is_authenticated:
                return redirect(url_for('auth.login'))
            system_roles = ["administrator", "operator", "personnel", "passenger"]
            if not user_role in system_roles:
                return redirect(url_for('passenger.search_connection_view'))
            if user_role == "passenger":
                for role in system_roles:
                    query = UserRoles.query.filter_by(user_id=current_user.id, role_name=role).first()
                    if query:
                        return func(*args, **kwargs)
                return redirect(url_for('passenger.search_connection_view'))
            elif user_role == "operator":
                # administrator,   allowed
                for role in system_roles[:2]:
                    query = UserRoles.query.filter_by(user_id=current_user.id, role_name=role).first()
                    if query:
                        return func(*args, **kwargs)
                return redirect(url_for('passenger.search_connection_view'))
            elif user_role == "personnel":
                # administrator, operator, personnel allowed
                for role in system_roles[:3]:
                    query = UserRoles.query.filter_by(user_id=current_user.id, role_name=role).first()
                    if query:
                        return func(*args, **kwargs)
                return redirect(url_for('passenger.search_connection_view'))
            elif user_role == "administrator":
                if UserRoles.query.filter_by(user_id=current_user.id, role_name=user_role).first():
                    return func(*args, **kwargs)
                return redirect(url_for('passenger.search_connection_view'))
            else:
                return redirect(url_for('passenger.search_connection_view'))
        return wrapped_function
    return decorator


@auth.route("/register", methods=["GET", "POST"])
def register():
    if current_user.is_authenticated:
        return redirect(url_for('passenger.search_connection_view')) 
    err = 0
    if request.method == "POST":
        user_data = {
            "login_name" :   request.form.get("login_name"),
            "password" :     request.form.get("password"),
            "password2" :    request.form.get("password2"),
            "user_name" :    request.form.get("user_name"),
            "user_surname" : request.form.get("user_surname"),
            "email" :        request.form.get("email"),
            "phone_number" : request.form.get("phone_number")
        }
        for _, v in user_data.items():
            if not v:
                err = 1
        if err == 1:
            return jsonify({'error': 'Invalid form, all entries must be filled'}), 210
        if user_data["password"] != user_data["password2"]:
            return jsonify({'error': 'Invalid form, passwords do not match.'}), 211
        if len(user_data["password"]) < 5:
            return jsonify({'error': 'Invalid form, the minimum password length must be longer than 5 characters.'}), 212
        user_email = User.query.filter_by(email=user_data["email"]).first()
        user_login = User.query.filter_by(login_name=user_data["login_name"]).first()
        if user_email or user_login:
            return jsonify({'error': 'Invalid form, login name or email already exists.'}), 213
        user = User(
            login_name=user_data["login_name"],
            email=user_data["email"],
            password=generate_password_hash(user_data["password"], method="sha256"),
            user_name=user_data["user_name"],
            user_surname=user_data["user_surname"],
            phone_number=user_data["phone_number"],
        )
        db.session.add(user)
        db.session.commit()
        added_user = User.query.filter_by(email=user_data["email"]).first()
        passenger_role = Role.query.filter_by(name="passenger").first()
        # this code should be moved to pre-specified roles classifier
        if passenger_role is None:
            passenger_role = Role(name="passenger")
        added_user.roles.append(passenger_role)
        db.session.commit()
        return jsonify({'data': f'Registration successfully completed, your login name: {user_data["login_name"]}, after 3 seconds you will be redirected to login page.'}), 200
    return render_template("register.html")


@auth.route("/logout", methods=["GET", "POST"])
@login_required
def logout():
    logout_user()
    return redirect(url_for('views.search')) 


@auth.route("/login", methods=["GET", "POST"])
def login():
    if current_user.is_authenticated:
        return redirect(url_for('passenger.search_connection_view')) 
    if request.method == "POST":
        login_or_email = request.form.get("login_email")
        password =  request.form.get("password")
        email_insert = User.query.filter_by(email=login_or_email).first()
        login_insert = User.query.filter_by(login_name=login_or_email).first()
        if not email_insert and not login_insert:
            return jsonify({'error': "Invalid form, user didn't exist."}), 210
        elif email_insert:
            if check_password_hash(email_insert.password, password):
                login_user(email_insert)
                session.permanent = True
                return jsonify({'data': f'Successfull login, welcome {email_insert.login_name}.'}), 200
            else:
                return jsonify({'error': "Invalid password."}), 211
        elif login_insert:
            if check_password_hash(login_insert.password, password):
                login_user(login_insert)
                session.permanent = True
                return jsonify({'data': f'Successfull login, welcome {login_insert.login_name}.'}), 200
            else:
                return jsonify({'error': "Invalid password."}), 211
    return render_template("login.html")


@auth.route("/get_nav_bar", methods=['GET'])
def get_navbar():
    user_data = ""
    if current_user.is_authenticated:
        # get role of logged user    
        for curr in ["administrator", "operator", "personnel", "passenger"]:
            current_role = UserRoles.query.filter_by(user_id=current_user.id, role_name=curr).first()
            if current_role:
                role = current_role.role_name
                break
        
        if role == 'administrator':
            left = nav_bar(
                nav_item('/administrator/confirm_connections', 'Confirm connections'),
                nav_item('/administrator/confirmation_to_stops', 'Confirm stop changes'),
                nav_item('/administrator/manage_users', 'Manage user accounts'),
                
                drop_down('Other actions',
                    drop_item('/operator/design_infrastructure', 'Design infrastructure'),
                    drop_item('/operator/connections', 'Manage connections'),
                    drop_item('/operator/manage_vehicles', 'Manage vehicles'),
                    drop_item('/operator/manage_personnel', 'Manage personnel accounts'),
                    drop_separator(),
                    drop_item('/personnel/manage_tickets', 'Manage tickets'),
                    drop_item('/personnel/update_connection_location', 'Update connection location'),
                    drop_separator(),
                    drop_item('/passenger/search_connection', 'Search connection'),
                    drop_item('/search_reservations', 'Search reservations')
                )
            )
        elif role == 'operator':
            left = nav_bar(
                nav_item('/operator/design_infrastructure', 'Design infrastructure'),
                nav_item('/operator/connections', 'Connections'),
                nav_item('/operator/manage_vehicles', 'Vehicles'),
                nav_item('/operator/manage_personnel', 'Personnel accounts'),
                drop_down('Other actions',
                    drop_item('/personnel/manage_tickets', 'Manage tickets'),
                    drop_item('/personnel/update_connection_location', 'Update connection location'),
                    drop_separator(),
                    drop_item('/passenger/search_connection', 'Search connection'),
                    drop_item('/search_reservations', 'Search reservations')
                )
            )
        elif role == 'personnel':
            left = nav_bar(
                nav_item('/personnel/manage_tickets', 'Manage tickets'),
                nav_item('/personnel/update_connection_location', 'Update connection location'),
                drop_down('Other actions',
                    drop_item('/passenger/search_connection', 'Search connection'),
                    drop_item('/search_reservations', 'Search reservations')
                )
            )
        elif role == 'passenger':
            left = nav_bar(
                nav_item('/passenger/search_connection', 'Search connection'),
                nav_item('/search_reservations', 'Search reservations')
            )

        right = nav_item('/logout', 'Logout')
        user_data = user_info(current_user, curr, "user-info")
    else:
        left = nav_bar(
            nav_item('/passenger/search_connection', 'Search connection'),
            nav_item('/search_reservations', 'Search reservations')
        )
        right = nav_bar(
            nav_item('/register', 'Registration'), 
            nav_item('/login', 'Login')
            )
    
    # JQS to append navbar items
    script = f"document.getElementById('left-nav-bar').innerHTML += '{left}';{user_data}document.getElementById('right-nav-bar').innerHTML +='{right}';"
    return script
    


def user_info(user, role, div_id):
    info = f"document.getElementById('{div_id}').innerHTML += "+ "'" + '<i class="bi bi-person-check-fill" style="font-size:40px;"></i>' + "';" 
    info += "var popover = new bootstrap.Popover(document.querySelector('#user-info')" + ",{trigger:'hover', container : 'body'});"
    info += f"document.querySelector('#user-info').setAttribute('data-bs-content', '<p>User-name: <b>{user.login_name}</b></p> <p>User-email: <b>{user.email}</b></p> <p>User-role: <b>{role}</b></p>');"
    return info
# macro to generate html string for nav item
def nav_item(href, caption):
    return f'<li class="my-auto nav-item mx-5"><a class="nav-link h4" href="{href}">{caption}</a></li>'

# macro to generate html string for nav bar from nav items 
def nav_bar(*nav_items):
    return ''.join(nav_items)

def drop_down(caption, *drop_items):
    return f'<li class="my-auto nav-item mx-5 dropdown"><a class="nav-link h4 dropdown-toggle" href="#" role="button" data-bs-toggle="dropdown">{caption}</a><ul class="dropdown-menu rounded bg-primary"><li>{"</li><li>".join(drop_items)}</li></ul></li>'

def drop_item(href, caption):
    return f'<a class="dropdown-item h5 text-light" href="{href}">{caption}</a>'

def drop_separator():
    return '<hr class="dropdown-divider">'
