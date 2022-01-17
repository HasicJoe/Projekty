from flask import Blueprint, json, render_template, request, flash, jsonify, Response
from . import db
from .auth import user_privileges

administrator = Blueprint('administrator', __name__)

# Aministrator use-cases
@administrator.route('/manage_users', methods=['GET', 'POST'])
@user_privileges(user_role="administrator") 
def manage_users_view():
    if request.method == 'POST':
        id =            request.form.get('text-id')
        login_name =    request.form.get('text-login-name')
        user_name =     request.form.get('text-user-name')
        user_surname =  request.form.get('text-user-surname')
        email =         request.form.get('text-email')
        phone_number =  request.form.get('text-phone-number')

        # ajax link, when specified in render template, will POST without refreshing page
        link = request.url[len(request.host_url)-1:]
        
        # check data integrity
        from .models import User

        user =                  User.query.filter_by(id=id).first()
        login_name_not_unique = User.query.filter_by(login_name=login_name).first()
        email_not_unique =      User.query.filter_by(email=email).first()

        # internal error, user with id @id does not exist
        if not user:
            return Response(render_template('admin_views/manage_user_accounts.html'), status=210)
        # entered login name or email was not unique
        elif login_name_not_unique and user.login_name != login_name:
            return Response(render_template('admin_views/manage_user_accounts.html'), status=211)
        elif email_not_unique and user.email != email:
            return Response(render_template('admin_views/manage_user_accounts.html'), status=212)
        # testing that all fields are entered
        elif not id or not login_name or not user_name or not user_surname or not email or not phone_number:
            return Response(render_template('admin_views/manage_user_accounts.html'), status=213)
        # Success: propagate changes to the database
        else:
            user.login_name =   login_name
            user.user_name =    user_name
            user.user_surname = user_surname
            user.email =        email
            user.phone_number = phone_number

            # commit db
            db.session.commit()

            return Response(render_template( 'admin_views/manage_user_accounts.html'), status=200)

    return render_template('admin_views/manage_user_accounts.html')

# JSON Data for manage_users_view
@administrator.route('/manage_users_data')
@user_privileges(user_role="administrator") 
def manage_users_data():
    from .models import User
    # query all passengers
    user_data = User.query.join(User.roles).all()
    return jsonify({ 'data' : user_data })

# delete user
@administrator.route('/delete_users', methods=["POST"])
@user_privileges(user_role="administrator") 
def delete_users():
    from .models import User, Reservation
    if request.method == 'POST':
        user = User.query.filter_by(id=int(request.args['id'])).first()
        user_reservations = Reservation.query.filter_by(user_id=int(request.args["id"])).all()
        if user_reservations:
            for reservation in user_reservations:
                reservation.email = user.email
                reservation.user_id = None
            db.session.commit()
        User.query.filter_by(id=int(request.args['id'])).delete()
        db.session.commit()
        return render_template('admin_views/manage_user_accounts.html')

@administrator.route('/confirmation_to_stops')
@user_privileges(user_role="administrator") 
def confirmation_to_stops_view():
    return render_template('admin_views/confirm_stop_changes.html')

@administrator.route('/deny_change', methods = ['POST'])
@user_privileges(user_role="administrator") 
def deny_change():
    if request.method == 'POST':
        stop_name = request.form.get('edit-new-name')
        request_name = f'Edit request-{stop_name}'
        if not stop_name:
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=210)
        else:
            from .models import Stop
            Stop.query.filter_by(name=request_name, status=2).delete()
            db.session.commit()
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=200)


@administrator.route('/accept_change', methods = ['POST'])
@user_privileges(user_role="administrator") 
def accept_change():
    if request.method == 'POST':
        stop_name = request.form.get('edit-new-name')
        request_name = f'Edit request-{stop_name}'
        new_addr = request.form.get('edit-new-address')
        new_lat = request.form.get('edit-new-latitude')
        new_long = request.form.get('edit-new-longitude')
        new_desc = request.form.get('edit-new-description')
        if not stop_name or not request_name:
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=210)
        elif not new_addr or not new_lat or not new_long or not new_desc:
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=211)
        else:
            from .models import Stop
            query_stop = Stop.query.filter_by(name=stop_name).first()
            if not query_stop:
                return Response(render_template('admin_views/confirm_stop_changes.html'), status=212)
            Stop.query.filter_by(name=request_name).delete()
            query_stop.address = new_addr
            query_stop.latitude = new_lat
            query_stop.longitude = new_long
            query_stop.description = new_desc
            db.session.commit()
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=200)

@administrator.route('/waiting_requests', methods=["GET","POST"])
@user_privileges(user_role="administrator") 
def get_waiting_requests():
    from .models import Stop
    data = Stop.query.filter_by(status=0).all()
    return jsonify({ 'data' : data })

@administrator.route("/requests_to_edit", methods=["GET", "POST"])
@user_privileges(user_role="administrator") 
def get_edit_requests():
    from .models import Stop

    resp_data = []
    data = Stop.query.filter_by(status=2).all()
    for row in data:
        old_data = Stop.query.filter_by(
            name=str(row.name).replace("Edit request-", "")
        ).first()
        if old_data:
            resp_data.append(
                {
                    "name": old_data.name,
                    "old_addr" : old_data.address,
                    "new_addr" : row.address,
                    "old_lat": old_data.latitude,
                    "new_lat": row.latitude,
                    "old_long": old_data.longitude,
                    "new_long": row.longitude,
                    "old_desc": old_data.description,
                    "new_desc": row.description,
                    "edit_name": row.name,
                    "request" : str(old_data.address + " -> " + row.address)
                }
            )
    return jsonify({"data": resp_data})

@administrator.route('/requests_to_delete', methods=["GET","POST"])
@user_privileges(user_role="administrator") 
def get_deleting_requests():
    from .models import Stop
    data = Stop.query.filter_by(status=3).all()
    return jsonify({ 'data' : data })

@administrator.route('/confirm_request', methods=["POST"])
@user_privileges(user_role="administrator") 
def confirm_request():
    from .models import Stop
    if request.method == 'POST':
        if not request.args['address']:
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=210)
        else:
            stop = Stop.query.filter_by(name=request.args['address']).first()
            stop.status = 1
            db.session.commit()
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=200)
    else:
        return render_template('admin_views/confirm_stop_changes.html')

@administrator.route('/delete_request', methods=["POST"])
@user_privileges(user_role="administrator") 
def delete_request():
    from .models import Stop
    if request.method == 'POST':
        if not request.args['address']:
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=210)
        else:
            Stop.query.filter_by(name=request.args['address']).delete()
            db.session.commit()
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=200)
    else:
        return render_template('admin_views/confirm_stop_changes.html')

@administrator.route('/delete_request_with_copy', methods = ["POST"])
@user_privileges(user_role="administrator") 
def delete_request_with_copy():
    from .models import Stop, Link, Route
    if request.method == 'POST':
        if not request.args['name']:
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=210)
        else:
            original_name = request.args['name'].replace("Delete request-", "").strip()
            Stop.query.filter_by(name=request.args['name'].strip()).delete()
            routes = Route.query.filter_by(stop_id = original_name).all()
            link_ids = [route.link_id for route in routes]
            for link_id in link_ids:
                link_routes = Route.query.filter_by(link_id=link_id).all()
                if len(link_routes) > 2:
                    Route.query.filter_by(stop_id = original_name).delete()
                else:
                    Route.query.filter_by(stop_id = original_name).delete()
                    Link.query.filter_by(id=link_id).delete()
                db.session.commit()
            Stop.query.filter_by(name=original_name).delete()
            db.session.commit()
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=200)

@administrator.route('/keep_current', methods = ["POST"])
@user_privileges(user_role="administrator") 
def keep_current():
    from .models import Stop
    if request.method == "POST":
        if not request.args['address']:
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=210)
        else:
            Stop.query.filter_by(name=request.args['address']).delete()
            db.session.commit()
            return Response(render_template('admin_views/confirm_stop_changes.html'), status=200)


@administrator.route('/confirm_connections')
@user_privileges(user_role="administrator") 
def confirm_new_connections_view():
    return render_template('admin_views/confirm_connections.html')

@administrator.route('/new_connections_data', methods= ['GET'])
@user_privileges(user_role="administrator") 
def new_connections_data():
    from .models import Link
    
    # query only not confirmed and not edited connections
    link_data = Link.query.filter((Link.confirmed == False) & (Link.original_id == None)).all()
    return jsonify({'data' : link_data})

@administrator.route('/edited_connections_data', methods=['GET'])
@user_privileges(user_role="administrator") 
def edited_connections_data():
    from .models import Link

    # query only not confirmed and edited connetions
    link_data = Link.query.filter((Link.confirmed == False) & (Link.original_id != None)).all()
    return jsonify({ 'data' : link_data})

@administrator.route('/confirm_connection/<connection_id>', methods = ['GET', 'POST'])
@user_privileges(user_role="administrator") 
def confirm_connection(connection_id):
    from .models import Link
        
    connection = Link.query.get(connection_id)
    if not connection:
        return { 'status' : 'error', 'msg' : 'Internal server error', 'type' : 'danger' }
    
    # edited connection
    if connection.original_id is not None:

        from .models import Route,Seat, Current_location, Reservation
        Current_location.query.filter_by(connection_id=connection.original_id).delete()

        reservations = Reservation.query.filter_by(connection_id=connection.original_id).all()
        for reservation in reservations:
            Seat.query.filter_by(reservation_id=reservation.id).delete()
            db.session.delete(reservation)

        Route.query.filter_by(link_id=connection.original_id).delete()
        db.session.commit()

        original_id = connection.original_id
        connection.original_id = None
        Link.query.filter_by(id=original_id).delete()
    
    connection.confirmed = True

    db.session.commit() 
    
    return { 'status' : 'success', 'msg' : f'Connection: "{connection.link_name}" successfully confirmed', 'type' : 'success'}



@administrator.route('/abort_connection/<connection_id>', methods=['POST'])
@user_privileges(user_role="administrator") 
def abort_connection(connection_id):
    from .models import Link, Route
    
    # remove all routes connected to this link
    Route.query.filter_by(link_id=connection_id).delete()
    # update changes in db

    connection = Link.query.get(connection_id)
    if not connection:
        return { 'success' : False }

    db.session.commit()
    connection_name = connection.link_name + " "
    Link.query.filter_by(id=connection_id).delete()

    db.session.commit()

    return { 'success' : True, 'connection_name' : connection_name }

@administrator.route('/get_connection/<connection_id>', methods=['GET'])
@user_privileges(user_role="administrator") 
def get_connection_data(connection_id):
    from .models import Link, Vehicle, User, Route
    connection = Link.query.get(connection_id)
    if not connection:
        return { 'success' : False }

    vehicle = Vehicle.query.get(connection.vehicle_id)
    operator = User.query.get(connection.operator)
    routes = Route.query.filter_by(link_id=connection.id).all()

    if not vehicle or not operator or not routes:
        return { 'success' : False , 'vehicle' : vehicle, 'operator' : operator, 'routes' : routes}

    route_data = []
    for i, route in enumerate(routes):
        route_data.append({
            'stop_name' :       route.stop_id, 
            'arrival_time' :    route.arrival_time.strftime('%H:%M'),
            'departure_time' :  route.departure_time.strftime('%H:%M') })

    return {
        'success' : True,
        'connection_name' : connection.link_name,
        'original_id' :     connection.original_id,
        'operator_name' :   operator.user_name,
        'interval' :        connection.interval,
        'vehicle_license_plate' : vehicle.license_plate,
        'vehicle_first_class_capacity' : vehicle.first_class_capacity,
        'vehicle_second_class_capacity' : vehicle.second_class_capacity,
        'routes' : route_data
    }
    
